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

#define BUFFER_SIZE 4096

Client::Client(int fd) : _fd(fd) { fcntl(fd, F_SETFL, O_NONBLOCK); }

Client::~Client() { close(_fd); }

int Client::getFd() const { return _fd; }

const char* HTTP_RESPONSE =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "Hello, World!";

bool Client::sendDirectoryListings(const std::string& path) {
  std::string requestUri = _parser.getHttpRequest().getUri();
  std::string dirListingHtml = generateDirectoryListing(path, requestUri);

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

bool Client::sendDefaultPage() {
  _response.setStatusCode(200);
  _response.setFile("./default/index.html", "text/html", "inline");
  std::string responseString = _response.getResponse();
  send(_fd, responseString.c_str(), responseString.length(), 0);
  return true;
}

std::string Client::generateDirectoryListing(const std::string& path,
                                             const std::string& requestUri) {
  std::stringstream ss;
  ss << "<html><body><h1>Directory Listing for " << path << "</h1><ul>";
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    std::string fileName = entry.path().filename().string();
    std::string relativePath = requestUri + fileName;
    std::string displayName = fileName;
    if (std::filesystem::is_directory(entry.path())) {
      relativePath += "/";
      displayName += "/";
    }
    ss << "<li><a href=\"" << relativePath << "\">" << displayName
       << "</a></li>";
  }
  ss << "</ul></body></html>";
  return ss.str();
}

bool Client::handleContinue() {
  int status = _parser.handshake();
  if (status != 200 && status != 201) {
    Log::getInstance().error("Something went wrong while processing request: " +
                             _parser.getHttpRequest().getHost());
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

bool Client::execute() {
  auto request = _parser.getHttpRequest();
  if (request.getHandler() == HttpRequestHandler::CGI) {
    Log::getInstance().debug("Successful request. CGI");
    auto cgi = std::make_shared<CGI>(_fd, request);
    if (cgi->run()) Event::getInstance().addEvent(_fd, cgi);
  } else if (request.getHandler() == HttpRequestHandler::FAVICON) {
    Log::getInstance().debug("Successful request. Favicon");
    sendDefaultFavicon();
  } else if (request.getHandler() == HttpRequestHandler::STATIC &&
             request.getMethodEnum() == HttpRequestMethod::GET &&
             request.getUri() == "/") {
    Log::getInstance().debug("Successful request. Static");
    sendDefaultPage();
  } else if (request.getHandler() == HttpRequestHandler::DIRECTORY_LISTING) {
    Log::getInstance().debug("Successful request. Directory Listing");
    sendDirectoryListings("./default" + request.getUri());
  } else {
    send(_fd, HTTP_RESPONSE, strlen(HTTP_RESPONSE), 0);
  }
  return true;
}

bool Client::handleRequest() {
  char buffer[BUFFER_SIZE];

  int bytes_read;
  int status;

  while (true) {
    if (_parser.status == HttpRequestParseStatus::EXPECT_CONTINUE) {
      return handleContinue();
    }
    bytes_read = read(_fd, buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      std::cout << buffer << std::endl;
      _parser = HttpRequestParser(buffer, _fd);
      status = _parser.parse();
      if ((status == 200 || status == 201) &&
          _parser.status == HttpRequestParseStatus::PARSED) {
        return execute();
      } else if ((status == 200 || status == 201) &&
                 _parser.status == HttpRequestParseStatus::EXPECT_CONTINUE) {
        Log::getInstance().debug("Request is to be continued: " +
                                 _parser.getHttpRequest().getHost());
        return true;
      }
      auto request = _parser.getHttpRequest();
      Log::getInstance().error(
          "Something went wrong while processing request: " +
          std::string(buffer));
      _response.setStatusCode(status);
      std::string responseString = _response.getResponse();
      send(_fd, responseString.c_str(), responseString.length(), 0);
      close(_fd);
      return false;
    } else if (bytes_read < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      continue;
    } else {
      if (bytes_read < 0) {
        throw readFailed(std::strerror(errno));
      }
      close(_fd);
      return false;
    }
  }
}
