#include "../../include/request/HttpRequestParser.hpp"  //also wouldn t recognize the path

#include <sys/socket.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <utility>

#include "../../include/log/Log.hpp"
#include "../../include/request/Helpers.hpp"
#include "../../include/request/HttpRequestEnums.hpp"
#include "../../include/response/HttpResponse.hpp"
#define MAX_BUFFER_SIZE 4096
#define UPLOAD_DIR "uploads/"
#define MAX_HEADER_SIZE 4096

#define DIR_LIST_ON 1

using namespace std::filesystem;
HttpRequestParser::HttpRequestParser()
    : status(HttpRequestParseStatus::NOT_PARSED) {}

HttpRequestParser::HttpRequestParser(
    const std::string raw, int clientFd,
    std::shared_ptr<ServerContext> serverContext)
    : status(HttpRequestParseStatus::NOT_PARSED),
      hasFile(false),
      _clientFd(clientFd),
      _raw(raw) {
  _request = HttpRequest();
  _boundary = "";
  total_read = 0;
  currentFileUploadStatus = HttpFileUploadStatus::NOT_STARTED;
  currentFileUploadName = "";
  _statusCode = 0;
  _serverContext = serverContext;
}

HttpRequestParser::~HttpRequestParser() {}

HttpRequest HttpRequestParser::getHttpRequest() {
  if (status == HttpRequestParseStatus::PARSED) {
    return _request;
  } else {
    return HttpRequest();
  }
}

std::shared_ptr<Location> HttpRequestParser::getMostRelevantLocation() {
  for (auto &location : _serverContext->_locationContext) {
    if (location->_urlValue == _request.getUri() ||
        location->_urlValue == _request.getUri() + "/") {
      return location;
    }
  }
  for (auto &location : _serverContext->_locationContext) {
    // check if _urlValue is contained inside _request.getUri() starting from
    // the beginning of the string
    if (_request.getUri().find(location->_urlValue) == 0) {
      return location;
    }
  }
  return nullptr;
}

/**
 * Determines the appropriate handler for the HTTP request.
 *
 * This function analyzes the request and sets the appropriate handler
 * based on the request's URI and method. It checks for specific conditions
 * and sets the handler accordingly. If a relevant location is found, it
 * sets the handler to RETURN and returns false.
 *
 * @return true if the handler is of type that requires further processing,
 * otherwise false
 */
bool HttpRequestParser::electHandler() {
  auto location = getMostRelevantLocation();
  if (location != nullptr) {
    if (location->_returnSet) {
      setStatusCode(location->_returnValues.first);
      setLocation(location->_returnValues.second);
      _request.setHandler(HttpRequestHandler::RETURN);
      return false;  // return false to prevent further processing
    }
  }
  if (isCgiRequest()) {
    _request.setHandler(HttpRequestHandler::CGI);
  } else if (isFaviconRequest()) {
    _request.setHandler(HttpRequestHandler::FAVICON);
    Log::getInstance().debug("Favicon handler for request: " +
                             _request.getUri());
  } else if (isDirectoryRequest("./default" + _request.getUri()) &&
             DIR_LIST_ON) {
    _request.setHandler(HttpRequestHandler::DIRECTORY_LISTING);
  } else if (_request.getUri() == "/uploads" &&
             _request.getMethodEnum() == HttpRequestMethod::GET) {
    _request.setHandler(HttpRequestHandler::LIST_UPLOADS);
  } else if (_request.getUri().substr(0, 9) == "/uploads/" &&
             _request.getMethodEnum() == HttpRequestMethod::GET) {
    _request.setHandler(HttpRequestHandler::SEND_UPLOADED_FILE);
  } else if (_request.getUri() == "/uploads" &&
             _request.getMethodEnum() == HttpRequestMethod::POST) {
    _request.setHandler(HttpRequestHandler::FILE_UPLOAD);
  } else if (_request.getUri() == "/benchmark") {
    _request.setHandler(HttpRequestHandler::BENCHMARK);
    setStatusCode(200);
    return false;  // return false to prevent further processing
  } else {
    _request.setHandler(HttpRequestHandler::STATIC);
  }
  return true;  // return true to continue processing
}

bool HttpRequestParser::isCgiRequest() {
  return _request.getUri().find("cgi-bin") != std::string::npos;
}

bool HttpRequestParser::isFaviconRequest() {
  return _request.getUri().find("favicon.ico") != std::string::npos;
}

bool HttpRequestParser::isDirectoryRequest(const std::string &path) {
  return is_directory(path);
}

std::string getLineSanitized(std::stringstream &ss) {
  std::string line;
  std::getline(ss, line);
  if (line.find("\r") != std::string::npos) {
    line.erase(line.find("\r"), 1);
  }
  return line;
}

bool HttpRequestParser::isAllowedMethod(const std::string &method) {
  auto location = getMostRelevantLocation();
  if (location != nullptr && location->_allowMethods.size() > 0) {
    for (auto &method : location->_allowMethods) {
      if (method == _request.getMethod()) {
        return true;
      }
    }
    return false;
  }
  // default allowed methods
  return method == "GET" || method == "POST" || method == "DELETE" ||
         method == "PUT" || method == "OPTIONS";
}

bool HttpRequestParser::isAllowedContentLength(size_t contentLength) {
  Log::getInstance().debug(
      "Max content length: " +
      std::to_string(_serverContext->_clientMaxBodySizeValue));
  return contentLength <= _serverContext->_clientMaxBodySizeValue;
}

bool HttpRequestParser::isUploadAllowed() {
  // TODO:: update this to use server configuration
  return _request.getUri().find("/upload") != std::string::npos &&
         _request.getMethodEnum() == HttpRequestMethod::POST;
}

void HttpRequestParser::injectUploadFormIfNeeded() {
  if (_request.getUri() == "/uploads" &&
      _request.getMethodEnum() == HttpRequestMethod::GET) {
    _request.addInjection("./default/upload/index.html");
  }
}

bool HttpRequestParser::canHaveBody() {
  return _request.getMethodEnum() == HttpRequestMethod::POST ||
         _request.getMethodEnum() == HttpRequestMethod::PUT ||
         _request.getMethodEnum() == HttpRequestMethod::DELETE ||
         _request.getMethodEnum() == HttpRequestMethod::PATCH;
}

int HttpRequestParser::parse() {
  std::stringstream ss(_raw);
  std::string requestLine = getLineSanitized(ss);
  if (!parseRequestLine((char *)requestLine.c_str(), requestLine.length())) {
    _request.setHandler(HttpRequestHandler::ERROR);
    return getStatusCode();
  }

  if (!electHandler()) {
    status = HttpRequestParseStatus::PARSED;
    return getStatusCode();
  }
  if (!parseHeaders(ss)) {
    _request.setHandler(HttpRequestHandler::ERROR);
    return getStatusCode();
  }
  size_t contentLength = _request.getContentLength();
  if (!isAllowedContentLength(contentLength)) {
    Log::getInstance().error("Invalid content length set by config: " +
                             std::to_string(contentLength));
    _request.setHandler(HttpRequestHandler::ERROR);
    setStatusCode(413);
    return 413;  // Payload Too Large
  }
  Log::getInstance().debug("Content length validated");
  injectUploadFormIfNeeded();
  if (_request.getHandler() == HttpRequestHandler::SEND_UPLOADED_FILE) {
    // check if file exists
    std::string filename =
        _request.getUri().substr(std::string(UPLOAD_DIR).length());
    if (!exists(UPLOAD_DIR + filename)) {
      Log::getInstance().error("File not found: " + filename);
      _request.setHandler(HttpRequestHandler::ERROR);
      setStatusCode(404);
      return 404;  // Not Found
    }
  }
  Log::getInstance().debug("Handler elected");
  size_t pos = _request.getUri().find("?");
  if (pos != std::string::npos) {
    _request.setQuery(_request.getUri().substr(pos + 1));
    _request.setUri(_request.getUri().substr(0, pos));
    parseQueryParams(_request.getQuery());
  }
  if (!canHaveBody() && contentLength > 0) {
    Log::getInstance().error("Request method " + _request.getMethod() +
                             " cannot have a body");
    _request.setHandler(HttpRequestHandler::ERROR);
    setStatusCode(405);
    return 400;
  }
  if (contentLength == 0) {
    setStatusCode(200);
    return 200;
  }
  std::string contentType = _request.getHeader("Content-Type");
  if (contentType.find("application/json") != std::string::npos ||
      contentType.find("text/plain") != std::string::npos) {
    if (!parseBody(ss)) {
      _request.setHandler(HttpRequestHandler::ERROR);
      return getStatusCode();
    }
  } else if (contentType.find("application/x-www-form-urlencoded") !=
             std::string::npos) {
    parseFormData(ss);
  } else if (hasFile && _request.getHeader("filename") != "" &&
             contentLength > 0) {
    if (!isUploadAllowed()) {
      Log::getInstance().error("File upload not allowed to endpoint " +
                               _request.getUri());
      _request.setHandler(HttpRequestHandler::ERROR);
      setStatusCode(405);
      return 405;  // Method Not Allowed
    }
    handleFileUpload(ss);
  }
  if (contentType.find("multipart/form-data") != std::string::npos) {
    if (!parseBoundary()) {
      _request.setHandler(HttpRequestHandler::ERROR);
      setStatusCode(400);
      return 400;
    }
    if (!handleFileUpload(ss)) {
      _request.setHandler(HttpRequestHandler::ERROR);
      setStatusCode(400);
      return 400;
    }
    if (!askForContinue() &&
        status == HttpRequestParseStatus::EXPECT_CONTINUE) {
      _request.setHandler(HttpRequestHandler::ERROR);
      setStatusCode(500);
      return 500;  // Internal Server Error
    }
  }
  setStatusCode(200);
  if (_request.getMethodEnum() == HttpRequestMethod::POST) {
    setStatusCode(201);
  }
  return getStatusCode();
}

bool HttpRequestParser::parseBoundary() {
  _boundary = _request.getHeader("Content-Type");
  std::string attr = "boundary=";
  size_t pos = _boundary.find(attr);
  if (pos == std::string::npos) {
    Log::getInstance().error(
        "Invalid multipart/form-data while parsing boundary for request " +
        _request.getUri());
    return false;
  }
  _boundary = _boundary.substr(pos + attr.length());
  return true;
}

bool HttpRequestParser::parseRequestLine(char *requestLine, size_t len) {
  size_t i = 0;
  for (i = 0; i < len; i++) {
    if (requestLine[i] == ' ') {
      bool valid = _request.setMethod(std::string(requestLine, i));
      if (!valid) {
        setStatusCode(400);
        return false;
      }
      break;
    }
  }
  size_t j = i + 1;
  for (i = j; i < len; i++) {
    if (requestLine[i] == ' ') {
      _request.setUri(std::string(requestLine + j, i - j));
      break;
    }
  }
  if (i == len || _request.getUri().empty() || _request.getUri()[0] != '/') {
    setStatusCode(400);
    Log::getInstance().error("Invalid URI " + _request.getUri());
    return false;
  }
  j = i + 1;
  _request.setHttpVersion(std::string(requestLine + j, len - j));
  if (!validateHttpVersion()) {
    Log::getInstance().error("Invalid HTTP version. expected HTTP/1.1 got " +
                             _request.getHttpVersion());
    setStatusCode(505);  // HTTP Version Not Supported
    return false;
  }
  if (!isAllowedMethod(_request.getMethod())) {
    Log::getInstance().error("Invalid method: " + _request.getMethod());
    setStatusCode(405);
    return false;
  }
  return true;
}

bool HttpRequestParser::parseHeaders(std::stringstream &ss) {
  std::string header;
  while ((header = getLineSanitized(ss)).length() > 0) {
    if (header.length() > MAX_HEADER_SIZE) {
      Log::getInstance().error("Header too long: " + header);
      setStatusCode(400);
      return false;
    }
    /*
    No whitespace is allowed between the field name and colon.  In the
    past, differences in the handling of such whitespace have led to
    security vulnerabilities in request routing and response handling.
    -- src: https://www.rfc-editor.org/rfc/inline-errata/rfc9112.html
    */
    if (header.find(" : ") != std::string::npos) {
      Log::getInstance().error(
          "Invalid header with extra space before colon: " + header);
      setStatusCode(400);
      return false;
    }
    size_t pos = header.find(": ");
    if (pos != std::string::npos) {
      std::string key = header.substr(0, pos);
      bool validHeader = HttpMaps::getInstance().isHeaderValid(key);
      if (!validHeader) {
        Log::getInstance().debug("Unknown header: " + key +
                                 " found in request " + _request.getUri());
        // Unknown headers are ignored to improve server performance and
        // prevent security vulnerabilities
        continue;
      }
      std::string value = header.substr(pos + 2);
      if (key == "Content-Disposition") {
        std::string attr = "filename=";
        std::size_t filePos = value.find(attr);
        if (filePos != std::string::npos) {
          hasFile = true;
          std::string filename = value.substr(filePos + attr.length());
          if (filename.front() == '"' && filename.back() == '"') {
            filename = filename.substr(1, filename.length() - 2);
          }
          Log::getInstance().debug("File upload detected: " + filename);
          key = "filename";
          value = filename;
        }
      }
      _request.setHeader(key, value);
    } else {
      Log::getInstance().error("Invalid header: " + header);
      setStatusCode(400);
      return false;
    }
  }
  std::string host = _request.getHeader("Host");
  if (host != "") {
    size_t pos = host.find(":");
    if (pos != std::string::npos) {
      _request.setHost(host.substr(0, pos));
      _request.setPort(host.substr(pos + 1));
    } else {
      _request.setHost(host);
      _request.setPort("80");
    }
    status = HttpRequestParseStatus::PARSED;
    setStatusCode(200);
    return true;
  }
  setStatusCode(400);
  return false;
}

bool HttpRequestParser::parseBody(std::stringstream &ss) {
  if (_request.getMethodEnum() == HttpRequestMethod::GET ||
      _request.getMethodEnum() == HttpRequestMethod::HEAD ||
      _request.getMethodEnum() == HttpRequestMethod::OPTIONS) {
    // While it's not strictly forbidden to send a body in a GET request,
    // it's not recommended and it's not supported by most servers.
    // This is a design decision, and it's not a requirement of the HTTP
    setStatusCode(405);
    return false;
  }
  size_t contentLength = _request.getContentLength();
  std::string body;
  std::string line;
  while (std::getline(ss, line)) {
    body += line;
  }
  if (contentLength != body.length()) {
    Log::getInstance().error("Invalid content length " +
                             std::to_string(contentLength));
    setStatusCode(400);
    return false;
  }
  _request.setBody(body);
  return true;
}

void HttpRequestParser::parseFormData(std::stringstream &ss) {
  /*
  /r/n
  username=john%20doe&password=mypassword
  */
  std::string data;
  while (std::getline(ss, data)) {
    if (data == "\r" || data == "") {
      continue;
    }
    std::string fact;
    std::stringstream facts(Helpers::decodeUrl(data));
    while (std::getline(facts, fact, '&')) {
      if (fact.find("\r") != std::string::npos)
        fact = fact.erase(data.find("\r"), 1);
      size_t pos = fact.find("=");
      if (pos != std::string::npos) {
        std::string key = fact.substr(0, pos);
        std::string value = fact.substr(pos + 1);
        _request.addFormData(key, value);
      } else {
        _request.addFormData(data, "");
      }
    }
  }
}

bool HttpRequestParser::handleFileUpload(std::stringstream &ss) {
  std::string contentType = _request.getHeader("Content-Type");
  if (contentType.find("application/octet-stream") != std::string::npos) {
    return handleOctetStream(ss);
  } else if (contentType.find("multipart/form-data") != std::string::npos) {
    return handleMultipartFormData(ss);
  }
  return false;
}

bool HttpRequestParser::handleOctetStream(std::stringstream &ss) {
  std::string data;
  int contentLength = _request.getContentLength();
  int bytesWritten = 0;
  std::string filename = _request.getHeader("filename");
  std::ofstream file(UPLOAD_DIR + filename, std::ios::binary);
  if (!file.is_open()) {
    Log::getInstance().error("Failed to open file for writing");
    return false;
  }
  status = HttpRequestParseStatus::EXPECT_CONTINUE;
  while (std::getline(ss, data)) {
    file << data.data();
    bytesWritten += data.length();
    file << "\n";
    bytesWritten++;
    if (bytesWritten >= contentLength) {
      Log::getInstance().debug("File upload complete for request: " +
                               _request.getUri());
      file.close();
      status = HttpRequestParseStatus::PARSED;
      return true;
    }
  }

  char buffer[MAX_BUFFER_SIZE + 1];
  int bytes_read = read(_clientFd, buffer, MAX_BUFFER_SIZE);
  while (bytes_read > 0) {
    file.write(buffer, bytes_read);
    bytesWritten += bytes_read;
    if (bytesWritten >= contentLength) {
      break;
    }
    bytes_read = read(_clientFd, buffer, MAX_BUFFER_SIZE);
  }
  file.close();
  status = HttpRequestParseStatus::PARSED;
  if (!askForContinue()) {
    return false;
  }
  return true;
}

bool HttpRequestParser::askForContinue() {
  std::string expectation = _request.getHeader("Expect");
  Log::getInstance().debug("Expectation: " + expectation);
  if (expectation == "100-continue") {
    Log::getInstance().debug("Client sent header Expect: 100-continue");
    HttpResponse response = HttpResponse(100);
    response.setHeader("Connection", "keep-alive");
    std::string responseString = response.getResponse();
    status = HttpRequestParseStatus::EXPECT_CONTINUE;
    if (send(_clientFd, responseString.c_str(), responseString.length(), 0) <
        0) {
      Log::getInstance().error("Failed to send 100 Continue response");
      return false;
    }
    return true;
  }
  status = HttpRequestParseStatus::PARSED;
  return true;
}

int readMore(std::stringstream &ss, int _clientFd) {
  char buffer[MAX_BUFFER_SIZE + 1];
  int bytes_read = 0;
  bytes_read = read(_clientFd, buffer, MAX_BUFFER_SIZE);
  if (bytes_read > 0) {
    buffer[bytes_read] = '\0';
    ss << buffer;
  } else {
    if (bytes_read < 0) {
      Log::getInstance().error("Readmore failed with error: " +
                               std::string(std::strerror(errno)));
    }
  }
  return bytes_read;
}

bool HttpRequestParser::checkForTerminator(std::string line) {
  if (line.find(_boundary + "--") != std::string::npos) {
    status = HttpRequestParseStatus::PARSED;
    Log::getInstance().debug("Multipart form data parsed for request: " +
                             _request.getUri());
    currentFileUploadStatus = HttpFileUploadStatus::COMPLETE;
    return true;
  }
  return false;
}

bool HttpRequestParser::handleMultipartFormData(std::stringstream &ss) {
  std::string data;
  int bytes_read = 0;
  size_t contentLength = _request.getContentLength();
  if (status == HttpRequestParseStatus::EXPECT_CONTINUE) {
    bytes_read = readMore(ss, _clientFd);
    Log::getInstance().debug("Bytes read: " + std::to_string(bytes_read));
    if (bytes_read == -1) return false;
    total_read += bytes_read;
  }
  // set the status to EXPECT_CONTINUE anyways because not every request
  // library will send the Expect header
  status = HttpRequestParseStatus::EXPECT_CONTINUE;
  if (_boundary.empty()) parseBoundary();
  while ((data = getLineSanitized(ss)).length() > 0) {
    if (checkForTerminator(data)) break;
    std::string filename;
    std::string key;
    std::string contentType;
    while ((data = getLineSanitized(ss)).length() > 0) {
      if (data.find(_boundary) != std::string::npos) continue;
      key = Helpers::getFormKeyIfExists(data);
      if (key.empty()) {
        filename = Helpers::getFilenameIfExists(data);
        std::getline(ss, data);
        if (data.find("Content-Type: ") != std::string::npos) {
          contentType = data.substr(14);
          Log::getInstance().debug("Content-Type: " + contentType);
        }
      } else {
        std::getline(ss, data);  // skip empty line
        std::getline(ss, data);
        if (checkForTerminator(data)) break;
        Log::getInstance().debug("Adding form data: " + key + " " + data);
        _request.addFormData(key, data);
        key = "";
        continue;
      }
      if (filename.empty() && key.empty()) {
        status = HttpRequestParseStatus::INVALID;
        Log::getInstance().error(
            "Invalid multipart/form-data while checking for filename and "
            "key");
        return false;
      }
      std::ofstream file(UPLOAD_DIR + filename, std::ios::binary);
      if (!file.is_open()) {
        Log::getInstance().error("Failed to open file for writing");
        return false;
      }
      currentFileUploadName = filename;
      currentFileUploadStatus = HttpFileUploadStatus::IN_PROGRESS;
      Log::getInstance().debug("Writing to file: " + filename);
      std::getline(ss, data);  // skip empty line
      // getLineSanitized is not used here because we need to write to the
      // file even if the line is empty
      bool boundaryFound = false;
      while (std::getline(ss, data)) {
        if (data.find("\r") != std::string::npos)
          data = data.erase(data.find("\r"), 1);
        if (checkForTerminator(data) ||
            data.find(_boundary) != std::string::npos) {
          Log::getInstance().debug("Boundary found!!");
          boundaryFound = true;
          break;
        }
        file << data;
        if (!Helpers::boundaryUpcoming(ss, _boundary)) file << "\n";
        if (bytes_read == MAX_BUFFER_SIZE) {
          bytes_read = readMore(ss, _clientFd);
          total_read += bytes_read;
        }
      }
      file.close();
      Log::getInstance().debug("Boundary found: " +
                               std::to_string(boundaryFound));
      if (total_read > contentLength) {
        Log::getInstance().error("Content-Length exceeded");
        return false;
      }
      _request.addAttachment(filename, contentType);
      filename.clear();
      if (status == HttpRequestParseStatus::PARSED) return true;
    }
  }
  return true;
}

int HttpRequestParser::handshake() {
  Log::getInstance().debug("--------------------------------");
  Log::getInstance().debug("Starting handshake for request: " +
                           _request.toJson());
  if (status == HttpRequestParseStatus::EXPECT_CONTINUE) {
    _request.setHeader("Expect", "");
    std::stringstream ss("");
    Log::getInstance().debug("File upload handler for request: " +
                             _request.getUri());
    if (!isUploadAllowed()) {
      Log::getInstance().error("File upload not allowed to " +
                               _request.getUri());
      setStatusCode(405);
      return 405;  // Method Not Allowed
    }
    if (handleFileUpload(ss)) {
      // TODO: handle when method is PUT and results in creating a new record
      // or file, should return 201
      if (_request.getMethodEnum() == HttpRequestMethod::POST) {
        setStatusCode(201);
        return 201;
      }
      setStatusCode(200);
      return 200;
    }
    setStatusCode(400);
    return 400;
  }
  setStatusCode(400);
  return 400;
}

bool HttpRequestParser::validateHttpVersion() {
  auto version =
      HttpMaps::getInstance().getVersionEnum(_request.getHttpVersion());
  _request.setHttpVersion(version);
  if (_request.getHttpVersionEnum() != HttpRequestVersion::HTTP_1_1)
    return false;
  return true;
}

void HttpRequestParser::parseQueryParams(std::string query) {
  std::stringstream ss(query);
  std::string param;
  while (std::getline(ss, param, '&')) {
    size_t pos = param.find("=");
    if (pos != std::string::npos) {
      std::string key = param.substr(0, pos);
      std::string value = param.substr(pos + 1);
      _request.setQueryParam(key, value);
    } else {
      _request.setQueryParam(param, "");
    }
  }
}

int HttpRequestParser::getStatusCode() { return _statusCode; }
void HttpRequestParser::setStatusCode(int code) { _statusCode = code; }
std::string HttpRequestParser::getLocation() { return _location; }
void HttpRequestParser::setLocation(std::string location) {
  _location = location;
}
