#include "../../include/networking/Client.hpp"

#include <fcntl.h>

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>

#include "../../include/Event.hpp"
#include "../../include/cgi/CGI.hpp"
#include "../../include/cgi/CGIFileManager.hpp"
#include "../../include/log/Log.hpp"
#include "ServerContext.hpp"

#define BUFFER_SIZE 4096
#define ERROR_PAGES "/default/error_pages/"
#define CONNECTION_TIMEOUT 5  // seconds before non-active connection is closed

Client::Client(int fd, std::shared_ptr<ServerContext> context) : _fd(fd) {
  _context = context;
  fcntl(fd, F_SETFL, O_NONBLOCK);
  _isReadyForResponse = false;
  _shouldSendContinue = false;
  _isReadyForRequest = true;
  _lastRequestTime = std::chrono::system_clock::now();
}

Client::~Client() { close(_fd); }

int Client::getFd() const { return _fd; }

void Client::reset() {
  std::string connection = _parser.getHttpRequest().getHeader("Connection");
  _shouldCloseConnection = false;
  if (connection == "close") {
    _shouldCloseConnection = true;
  }
  _parser = HttpRequestParser();
  _response = HttpResponse();
  _isReadyForResponse = false;
  _shouldSendContinue = false;
  _isReadyForRequest = true;
  _lastRequestTime = std::chrono::system_clock::now();
}

bool Client::sendDirectoryListings(const std::string& path) {
  auto req = _parser.getHttpRequest();
  std::string requestUri = req.getUri();
  std::string dirListingHtml =
      generateDirectoryListing(path, requestUri, req.getInjections());

  _response.setStatusCode(200);
  _response.setBody(dirListingHtml);
  _response.setContentType("text/html");

  std::string responseString = _response.getResponse();
  send(_fd, responseString.c_str(), responseString.length(), 0);
  return true;
}

bool Client::sendDefaultFavicon() {
  _response.setStatusCode(200);
  _response.setFile("./default/favicon-dt.png", "image/x-icon");
  std::string responseString = _response.getResponse();
  send(_fd, responseString.c_str(), responseString.length(), 0);
  return true;
}

std::string getErrorPagePath(int status) {
  return ERROR_PAGES + std::to_string(status) + "/" + std::to_string(status) +
         ".html";
}

void Client::sendErrorPage(int status) {
  // This section will handle the case where handler is set to ERROR.
  _response.setStatusCode(status);
  if (HttpMaps::getInstance().errorHasDefaultPage(status)) {
    // move to a redirect
    _response.setStatusCode(302);
    _response.setHeader("Location", getErrorPagePath(status));
  }
  std::string responseString = _response.getResponse();
  send(_fd, responseString.c_str(), responseString.length(), 0);
}

std::string getFirstValidIndexPage(std::vector<std::string> indexPages) {
  for (auto page : indexPages) {
    if (std::filesystem::exists("." + page)) {
      return "." + page;
    }
  }
  return "";
}

bool Client::sendWebDocument() {
  _response.setStatusCode(_parser.getStatusCode());
  std::string url = _parser.getHttpRequest().getUri();
  url = "." + url;
  // check if .url is a file
  if (std::filesystem::is_regular_file(url)) {
    std::string mimeType = HttpMaps::getInstance().getMimeType(url);
    _response.setFile(url, mimeType, "inline");
    std::string responseString = _response.getResponse();
    send(_fd, responseString.c_str(), responseString.length(), 0);
    return true;
  }
  std::cout << "URL: " << url << std::endl;
  if (url == "./" || url == "./index.html") {
	url = getFirstValidIndexPage(_context->_indexValue);
	std::cout << "Index page: " << url << std::endl;
	if (url == "") {
		if (_context->_indexValue.size() == 0)
    		url = "./default/index.html";
		else
			sendErrorPage(403);
	}
    _response.setFile(url, "text/html", "inline");
    std::string responseString = _response.getResponse();
    send(_fd, responseString.c_str(), responseString.length(), 0);
    return true;
  }
  if (url.back() == '/') sendErrorPage(403);
  sendErrorPage(404);
  return true;
}

std::string Client::generateDirectoryListing(const std::string& path,
                                             const std::string& requestUri,
                                             const std::string& inject) {
  std::stringstream ss;
  ss << "<html><body>";
  ss << inject;
  ss << "<h1>Directory Listing for " << path << "</h1><ul>";
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    std::string fileName = entry.path().filename().string();
    std::string relativePath = fileName;
    if (requestUri != "/") relativePath = requestUri + "/" + fileName;
    std::string displayName = fileName;
    if (std::filesystem::is_directory(entry.path())) {
      relativePath += "/";
      displayName += "/";
    }
    ss << "<li><a href=\"" << relativePath << "\">" << displayName
       << "</a></li>";
  }
  ss << "</ul></body></html>";
  Log::getInstance().debug("TO return directory listing: " + ss.str());
  return ss.str();
}

bool Client::handleContinue() {
  int status = _parser.handshake();
  if (status != 200 && status != 201) {
    Log::getInstance().error("Failed to complete handshake");
    _response.setStatusCode(status);
    std::string responseString = _response.getResponse();
    send(_fd, responseString.c_str(), responseString.length(), 0);
    close(_fd);
    return false;
  }
  if (_parser.status == HttpRequestParseStatus::PARSED) {
    auto request = _parser.getHttpRequest();
    _response.setStatusCode(status);
    std::string responseString = _response.getResponse();
    send(_fd, responseString.c_str(), responseString.length(), 0);
    close(_fd);
    Log::getInstance().debug(
        "Successful multipart/octet-stream request with handshake");
  }
  return true;
}

bool Client::handleRedirect() {
  _response.setStatusCode(_parser.getStatusCode());
  _response.setHeader("Location", _parser.getLocation());
  std::string responseString = _response.getResponse();
  send(_fd, responseString.c_str(), responseString.length(), 0);
  return true;
}

bool Client::shouldCloseConnection(bool force) {
  auto now = std::chrono::system_clock::now();
  auto diff =
      std::chrono::duration_cast<std::chrono::seconds>(now - _lastRequestTime);
  if (diff.count() >= 5 && _isReadyForRequest) {
    Log::getInstance().debug("Closing connection due to inactivity");
    return true;
  }
  if (force && diff.count() >= 10) {
    return true;
  }
  return _shouldCloseConnection;
}

bool Client::execute() {
  auto request = _parser.getHttpRequest();
  int status = _parser.getStatusCode();
  Log::getInstance().debug("Status code: " + std::to_string(status));
  switch (request.getHandler()) {
    case HttpRequestHandler::BENCHMARK: {
      _response.setStatusCode(200);
      std::string responseString = _response.getResponse();
      send(_fd, responseString.c_str(), responseString.length(), 0);
      break;
    }
    case HttpRequestHandler::RETURN: {
      Log::getInstance().debug("Successful request. Return");
      handleRedirect();
      break;
    }
    case HttpRequestHandler::FAVICON: {
      Log::getInstance().debug("Successful request. Favicon");
      sendDefaultFavicon();
      break;
    }
    case HttpRequestHandler::STATIC: {
      Log::getInstance().debug("Successful request. Static");
      sendWebDocument();
      break;
    }
    case HttpRequestHandler::CGI: {
      Log::getInstance().debug("Successful request. CGI");
      Event::getInstance().getEvent(_fd)->handleResponse();
      Event::getInstance().removeEvent(_fd);
      break;
    }
    case HttpRequestHandler::DIRECTORY_LISTING: {
      Log::getInstance().debug("Successful request. Directory Listing");
      sendDirectoryListings("." + request.getUri());
      break;
    }
    case HttpRequestHandler::LIST_UPLOADS: {
      Log::getInstance().debug("Successful request. List Uploads");
      sendDirectoryListings("./uploads");
      break;
    }
    case HttpRequestHandler::SEND_UPLOADED_FILE: {
      Log::getInstance().debug("Successful request. Send Uploaded File");
      _response.setStatusCode(200);
      std::string mimeType =
          HttpMaps::getInstance().getMimeType(request.getUri());
      _response.setFile("." + request.getUri(), mimeType);
      _response.sendResponse(_fd);
      break;
    }
    case HttpRequestHandler::FILE_UPLOAD: {
      _response.setStatusCode(302);
      _response.setHeader("Location", "/uploads");
      std::string responseString = _response.getResponse();
      Log::getInstance().debug(responseString);
      send(_fd, responseString.c_str(), responseString.length(), 0);
      break;
    }
    default:
      sendErrorPage(status);
      Log::getInstance().warning(
          request.getMethod() + " " + request.getHost() + request.getUri() +
          " failed with status: " + std::to_string(status));
      break;
  }
  Log::getInstance().info(request.getMethod() + " " + request.getHost() +
                          request.getUri());
  // clean up request/response objects
  reset();
  return true;
}

bool Client::handleRequest() {
  char buffer[BUFFER_SIZE + 1];

  int bytes_read;
  _isReadyForRequest = false;

  // if (_parser.status == HttpRequestParseStatus::EXPECT_CONTINUE) {
  //   return handleContinue();
  // }
  std::string raw = "";
  bytes_read = read(_fd, buffer, BUFFER_SIZE);
  if (bytes_read > 0) {
    buffer[bytes_read] = '\0';
    raw = buffer;
    while (bytes_read == BUFFER_SIZE) {
      raw.append(buffer, bytes_read);
      bytes_read = read(_fd, buffer, BUFFER_SIZE);
      if (bytes_read < 0) {
        Log::getInstance().error("Failed to read from socket with error: " +
                                 std::string(std::strerror(errno)));
        break;
      }
      buffer[bytes_read] = '\0';
    }
    _parser = HttpRequestParser(raw, _fd, _context);
    _parser.parse();
    Log::getInstance().debug("Parsed request: " + raw.substr(0, 100));
    if (_parser.status == HttpRequestParseStatus::EXPECT_CONTINUE) {
      Log::getInstance().debug("Request is to be continued: " +
                               raw.substr(0, 100));
      _shouldSendContinue = true;
    } else {
      _isReadyForResponse = true;
      if (_parser.getHttpRequest().getHandler() == HttpRequestHandler::CGI) {
        auto request = _parser.getHttpRequest();
        auto cgi = std::make_shared<CGI>(_fd, request);
        if (cgi->run()) Event::getInstance().addEvent(_fd, cgi);
        _isReadyForResponse = false;
      }
    }
    return true;
  } else {
    if (bytes_read < 0) {
      Log::getInstance().error("Failed to read from socket with error: " +
                               std::string(std::strerror(errno)));
    }
  }
  return true;
}

bool Client::isReadyForResponse() { return _isReadyForResponse; }
bool Client::shouldSendContinue() { return _shouldSendContinue; }
bool Client::isReadyForRequest() {
  return _isReadyForRequest && !_isReadyForResponse;
}

void Client::setReadyForResponse(bool ready) { _isReadyForResponse = ready; }
