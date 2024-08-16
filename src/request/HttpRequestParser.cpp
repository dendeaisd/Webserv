#include "../../include/request/HttpRequestParser.hpp"  //also wouldn t recognize the path

#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <utility>

#include "../../include/log/Log.hpp"
#include "../../include/request/HttpRequestEnums.hpp"

HttpRequestParser::HttpRequestParser()
    : status(HttpRequestParseStatus::NOT_PARSED) {}

HttpRequestParser::HttpRequestParser(const std::string request, int clientFd)
    : status(HttpRequestParseStatus::NOT_PARSED),
      hasFile(false),
      _clientFd(clientFd),
      raw(request) {}

HttpRequestParser::~HttpRequestParser() {}

HttpRequest HttpRequestParser::getHttpRequest() {
  if (status == HttpRequestParseStatus::PARSED) {
    return request;
  } else {
    return HttpRequest();
  }
}

bool isCGI(const std::string &uri) {
  return uri.find("cgi-bin") != std::string::npos;
}

void HttpRequestParser::electHandler() {
  // TODO: update this to use server configuration
  if (request.getUri().find("cgi-bin") != std::string::npos) {
    request.setHandler(HttpRequestHandler::CGI);
  } else {
    request.setHandler(HttpRequestHandler::STATIC);
  }
}

int HttpRequestParser::parse() {
  std::stringstream ss(raw);
  std::string requestLine;
  std::getline(ss, requestLine);
  std::cout << requestLine << std::endl;
  if (requestLine.find("\r") != std::string::npos)
    requestLine.erase(requestLine.find("\r"), 1);
  parseRequestLine((char *)requestLine.c_str(), requestLine.length());
  if (status == HttpRequestParseStatus::INVALID ||
      status == HttpRequestParseStatus::INCOMPLETE) {
    std::cout << "Invalid request line" << std::endl;
    return 400;
  }
  if (!validateHttpVersion()) {
    Log::getInstance().error(
        "Invalid HTTP version. expected HTTP/1.1 got " +
        request.getHttpVersion());
    return 505;  // HTTP Version Not Supported
  }
  parseHeaders(ss);
  if (status == HttpRequestParseStatus::INVALID) {
    std::cout << "Invalid headers" << std::endl;
    return 400;
  }
  std::string query;
  size_t pos = request.getUri().find("?");
  if (pos != std::string::npos) {
    request.setQuery(request.getUri().substr(pos + 1));
    request.setUri(request.getUri().substr(0, pos));
    parseQueryParams(query);
  }
  std::string contentType = request.getHeader("Content-Type");
  if (contentType.find("application/json") != std::string::npos) {
    parseBody(ss);

  } else if (contentType.find("application/x-www-form-urlencoded") !=
             std::string::npos) {
    parseFormData(ss);
  } else if (hasFile && request.getHeader("filename") != "") {
    handleFileUpload(ss);
  }
  if (contentType.find("multipart/form-data") != std::string::npos) {
    boundary = request.getHeader("Content-Type");
    std::string attr = "boundary=";
    size_t pos = boundary.find(attr);
    if (pos == std::string::npos) {
      Log::getInstance().error("Invalid multipart/form-data");
      return false;
    }
    boundary = boundary.substr(pos + attr.length());
    status = HttpRequestParseStatus::EXPECT_CONTINUE;
  }
  electHandler();
  return 200;
}

void HttpRequestParser::parseRequestLine(char *requestLine, size_t len) {
  size_t i = 0;
  for (i = 0; i < len; i++) {
    if (requestLine[i] == ' ') {
      bool valid = request.setMethod(std::string(requestLine, i));
      if (!valid) {
        status = HttpRequestParseStatus::INVALID;
        Log::getInstance().error("Invalid request method");
        return;
      }
      break;
    }
  }
  size_t j = i + 1;
  for (i = j; i < len; i++) {
    if (requestLine[i] == ' ') {
      request.setUri(std::string(requestLine + j, i - j));
      break;
    }
  }
  if (i == len || request.getUri().empty() || request.getUri()[0] != '/') {
    status = HttpRequestParseStatus::INCOMPLETE;
    Log::getInstance().error("Invalid URI");
    return;
  }
  j = i + 1;
  request.setHttpVersion(std::string(requestLine + j, len - j));
}

void HttpRequestParser::parseHeaders(std::stringstream &ss) {
  std::string header;
  while (std::getline(ss, header) && (header != "\r" && header != "")) {
    /*
    No whitespace is allowed between the field name and colon.  In the
    past, differences in the handling of such whitespace have led to
    security vulnerabilities in request routing and response handling.
    -- src: https://www.rfc-editor.org/rfc/inline-errata/rfc9112.html
    */
    if (header.find(" : ") != std::string::npos) {
      std::cout << "Invalid header" << std::endl;
      status = HttpRequestParseStatus::INVALID;
      return;
    }
    size_t pos = header.find(": ");
    if (pos != std::string::npos) {
      std::string key = header.substr(0, pos);
      if (HttpMaps::headerSet.find(key) == HttpMaps::headerSet.end()) {
        std::cout << "Unknown header" << std::endl;
        // Unknown headers are ignored to improve server performance and prevent
        // security vulnerabilities
        continue;
      }
      if (header.find("\r") != std::string::npos)
        header.erase(header.find("\r"), 1);
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
      request.setHeader(key, value);
    } else {
      status = HttpRequestParseStatus::INVALID;
      return;
    }
  }
  std::string host = request.getHeader("Host");
  if (host != "") {
    size_t pos = host.find(":");
    if (pos != std::string::npos) {
      request.setHost(host.substr(0, pos));
      request.setPort(host.substr(pos + 1));
    } else {
      request.setHost(host);
      request.setPort("80");
    }
    status = HttpRequestParseStatus::PARSED;
  } else {
    status = HttpRequestParseStatus::INVALID;
    return;
  }
}

void HttpRequestParser::parseBody(std::stringstream &ss) {
  if (request.getMethodEnum() == HttpRequestMethod::GET ||
      request.getMethodEnum() == HttpRequestMethod::DELETE) {
    // While it's not strictly forbidden to send a body in a GET request,
    // it's not recommended and it's not supported by most servers.
    // This is a design decision, and it's not a requirement of the HTTP
    status = HttpRequestParseStatus::INVALID;
    return;
  }
  std::string body;
  std::string line;
  while (std::getline(ss, line)) {
    body += line;
  }
  request.setBody(body);
}

std::string decodeUrl(const std::string &url) {
  std::string decoded;
  for (size_t i = 0; i < url.length(); i++) {
    if (url[i] == '%') {
      int value;
      sscanf(url.substr(i + 1, 2).c_str(), "%x", &value);
      decoded += static_cast<char>(value);
      i += 2;
    } else {
      decoded += url[i];
    }
  }
  return decoded;
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
    std::stringstream facts(decodeUrl(data));
    while (std::getline(facts, fact, '&')) {
      if (fact.find("\r") != std::string::npos)
        fact = fact.erase(data.find("\r"), 1);
      size_t pos = fact.find("=");
      if (pos != std::string::npos) {
        std::string key = fact.substr(0, pos);
        std::string value = fact.substr(pos + 1);
        request.addFormData(key, value);
      } else {
        request.addFormData(data, "");
      }
    }
  }
}

bool HttpRequestParser::handleFileUpload(std::stringstream &ss) {
  std::string contentType = request.getHeader("Content-Type");
  if (contentType.find("application/octet-stream") != std::string::npos) {
    return handleOctetStream(ss);
  } else if (contentType.find("multipart/form-data") != std::string::npos) {
    return handleMultipartFormData(ss);
  }
  return false;
}

bool HttpRequestParser::handleOctetStream(std::stringstream &ss) {
  std::string data;
  int contentLength = std::stoi(request.getHeader("Content-Length"));
  int bytesWritten = 0;
  std::string filename = request.getHeader("filename");
  std::string uploadDir = "uploads/";
  std::ofstream file(uploadDir + filename, std::ios::binary);
  if (!file.is_open()) {
    Log::getInstance().error("Failed to open file for writing");
    return false;
  }
  while (std::getline(ss, data)) {
    file << data.data();
    bytesWritten += data.length();
    file << "\n";
    bytesWritten++;
    if (bytesWritten >= contentLength) {
      file.close();
      return true;
    }
  }

  int toRead = 4096;
  if (contentLength < toRead) {
    toRead = contentLength;
  }
  char buffer[toRead + 1];
  int bytes_read = read(_clientFd, buffer, toRead);
  while (bytes_read > 0) {
    file.write(buffer, bytes_read);
    bytesWritten += bytes_read;
    if (bytesWritten >= contentLength) {
      break;
    }
    if (contentLength - bytesWritten < toRead) {
      toRead = contentLength - bytesWritten;
    }
    bytes_read = read(_clientFd, buffer, toRead);
    std::cout << "Bytes read: " << bytes_read << std::endl;
  }
  file.close();
  status = HttpRequestParseStatus::PARSED;
  std::string expectation = request.getHeader("Expect");
  std::cout << "Expect: " << expectation << std::endl;
  if (expectation == "100-continue") {
    std::string response = "HTTP/1.1 100 Continue\r\n\r\n";
    status = HttpRequestParseStatus::EXPECT_CONTINUE;
    if (send(_clientFd, response.c_str(), response.length(), 0) < 0) {
      Log::getInstance().error("Failed to send 100 Continue response");
      return false;
    }
    return true;
  }
  return true;
}

bool HttpRequestParser::handleMultipartFormData(std::stringstream &ss) {
  std::string data;
  if (status == HttpRequestParseStatus::EXPECT_CONTINUE) {
    char buffer[4096];
    int bytes_read = read(_clientFd, buffer, 4096);
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      ss << buffer;
      std::cout << "Buffer: " << buffer << std::endl;
    } else if (bytes_read == 0) {
      Log::getInstance().info("No data received");
      return true;
    } else {
      Log::getInstance().error("Failed to read from client");
      status = HttpRequestParseStatus::INVALID;
      return false;
    }
  }
  Log::getInstance().debug("Boundary: " + boundary);
  while (std::getline(ss, data)) {
    if (data.find("\r") != std::string::npos)
      data = data.erase(data.find("\r"), 1);
    if (data.find("--" + boundary + "--") != std::string::npos) {
      status = HttpRequestParseStatus::PARSED;
      Log::getInstance().debug("Multipart form data parsed");
      break;
    }
    if (data.find("--" + boundary) != std::string::npos) {
      Log::getInstance().debug("Boundary found");
    }
    std::string attr;
    std::string filename;
    std::string key;
    std::string contentType;
    while (std::getline(ss, data)) {
      if (data.find("\r") != std::string::npos)
        data = data.erase(data.find("\r"), 1);
      if (data == "") {
        Log::getInstance().debug("Empty line");
        continue;
      }
      if (data.find(boundary) != std::string::npos) {
        Log::getInstance().debug("Boundary found");
        continue;
      }
      Log::getInstance().debug("Line: " + data);
      if (data.find("form-data; ") != std::string::npos &&
          data.find("filename=") == std::string::npos) {
        attr = "name=";
        size_t pos = data.find(attr);
        key = data.substr(pos + attr.length());
        if (key.front() == '"' && key.back() == '"') {
          key = key.substr(1, key.length() - 2);
        }
        Log::getInstance().debug("Key: " + key);
      } else if (data.find("filename=") != std::string::npos) {
        attr = "filename=";
        size_t pos = data.find(attr);
        filename = data.substr(pos + attr.length());
        if (filename.front() == '"' && filename.back() == '"') {
          filename = filename.substr(1, filename.length() - 2);
        }
        Log::getInstance().debug("Filename: " + filename);
        std::getline(ss, data);
        if (data.find("Content-Type: ") != std::string::npos) {
          contentType = data.substr(14);
          Log::getInstance().debug("Content-Type: " + contentType);
        }
      }
      if (!key.empty()) {
        std::getline(ss, data);
        std::getline(ss, data);
        if (data.find("--" + boundary + "--") != std::string::npos) {
          break;
        }
        Log::getInstance().debug("Adding form data: " + key + " " + data);
        request.addFormData(key, data);
        key = "";
        continue;
      }
      if (filename.empty() && key.empty()) {
        Log::getInstance().error("Invalid multipart/form-data");
        return false;
      }
      std::string uploadDir = "uploads/";
      std::ofstream file(uploadDir + filename, std::ios::binary);
      if (!file.is_open()) {
        Log::getInstance().error("Failed to open file for writing");
        return false;
      }
      Log::getInstance().debug("Writing to file: " + filename);
      while (std::getline(ss, data)) {
        if (data.find("\r") != std::string::npos)
          data = data.erase(data.find("\r"), 1);
        if (data.empty()) {
          continue;
        }
        Log::getInstance().debug("Data: " + data);
        if (data.find(boundary + "--") != std::string::npos ||
            data.find(boundary) != std::string::npos) {
          if (data.find(boundary + "--") != std::string::npos) {
            status = HttpRequestParseStatus::PARSED;
            Log::getInstance().debug("Multipart form data parsed");
          }
          break;
        }
        file << data;
        file << "\n";
      }
      file.close();
      filename.clear();
    }
  }
  return true;
}

int HttpRequestParser::handshake() {
  Log::getInstance().debug("Handshake");
  if (status == HttpRequestParseStatus::EXPECT_CONTINUE) {
    request.setHeader("Expect", "");
    std::stringstream ss("");
    return handleFileUpload(ss) ? 200 : 400;
  }
  return 400;  // TODO: return appropriate status code
}

bool HttpRequestParser::validateHttpVersion() {
  if (HttpMaps::httpRequestVersionMap.find(request.getHttpVersion()) !=
      HttpMaps::httpRequestVersionMap.end()) {
    request.setHttpVersion(HttpMaps::httpRequestVersionMap.at(
        HttpMaps::httpRequestVersionMap.find(request.getHttpVersion())->first));
    if (request.getHttpVersionEnum() == HttpRequestVersion::HTTP_1_1) {
      return true;
    } else {
      return false;
    }
  } else {
    request.setHttpVersion(HttpRequestVersion::UNKNOWN);
    return false;
  }
}

void HttpRequestParser::parseQueryParams(std::string query) {
  std::stringstream ss(query);
  std::string param;
  while (std::getline(ss, param, '&')) {
    size_t pos = param.find("=");
    if (pos != std::string::npos) {
      std::string key = param.substr(0, pos);
      std::string value = param.substr(pos + 1);
      request.setQueryParam(key, value);
    } else {
      request.setQueryParam(param, "");
    }
  }
}
