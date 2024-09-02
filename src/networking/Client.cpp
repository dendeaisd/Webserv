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

Client::Client(int fd) : fd(fd) { fcntl(fd, F_SETFL, O_NONBLOCK); }

Client::~Client() { close(fd); }

int Client::getFd() const { return fd; }

const char* HTTP_RESPONSE =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "Hello, World!";

bool Client::sendDirectoryListings(const std::string& path) {
  auto req = parser.getHttpRequest();
  std::string requestUri = req.getUri();
  std::string dirListingHtml =
      generateDirectoryListing(path, requestUri, req.getInjections());

  response.setStatusCode(200);
  response.setBody(dirListingHtml);
  response.setContentType("text/html");

  std::string responseString = response.getResponse();
  send(fd, responseString.c_str(), responseString.length(), 0);
  return true;
}

bool Client::sendDefaultFavicon() {
  response.setStatusCode(200);
  response.setFile("./default/favicon-dt.png", "image/x-icon");
  std::string responseString = response.getResponse();
  send(fd, responseString.c_str(), responseString.length(), 0);
  return true;
}

bool Client::sendDefaultPage() {
  response.setStatusCode(200);
  response.setFile("./default/index.html", "text/html", "inline");
  std::string responseString = response.getResponse();
  send(fd, responseString.c_str(), responseString.length(), 0);
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
  return ss.str();
}

bool Client::handleContinue() {
  int status = parser.handshake();
  if (status != 200 && status != 201) {
    Log::getInstance().error("Something went wrong while processing request: " +
                             parser.getHttpRequest().getHost());
    response.setStatusCode(status);
    std::string responseString = response.getResponse();
    send(fd, responseString.c_str(), responseString.length(), 0);
    close(fd);
    return false;
  }
  if (parser.status == HttpRequestParseStatus::PARSED) {
    auto request = parser.getHttpRequest();
    response.setStatusCode(status);
    std::string responseString = response.getResponse();
    send(fd, responseString.c_str(), responseString.length(), 0);
    close(fd);
    Log::getInstance().debug(
        "Successful multipart/octet-stream request with handshake");
  }
  return true;
}

bool Client::execute() {
  auto request = parser.getHttpRequest();
  switch (request.getHandler()) {
    case HttpRequestHandler::CGI: {
      Log::getInstance().debug("Successful request. CGI");
      auto cgi = std::make_shared<CGI>(fd, request);
      if (cgi->run()) Event::getInstance().addEvent(fd, cgi);
      break;
    }
    case HttpRequestHandler::FAVICON: {
      Log::getInstance().debug("Successful request. Favicon");
      sendDefaultFavicon();
      break;
    }
    case HttpRequestHandler::STATIC: {
      sendDefaultPage();
      break;
    }
    case HttpRequestHandler::DIRECTORY_LISTING: {
      Log::getInstance().debug("Successful request. Directory Listing");
      sendDirectoryListings("./default" + request.getUri());
      break;
    }
    case HttpRequestHandler::LIST_UPLOADS: {
      Log::getInstance().debug("Successful request. List Uploads");
      sendDirectoryListings("./uploads");
      break;
    }
    case HttpRequestHandler::SEND_UPLOADED_FILE: {
      Log::getInstance().debug("Successful request. Send Uploaded File");
      response.setStatusCode(200);
      std::string mimeType = HttpMaps::getMimeType(request.getUri());
      response.setFile("." + request.getUri(), mimeType);
      response.sendResponse(fd);
      break;
    }
    default: {
      send(fd, HTTP_RESPONSE, strlen(HTTP_RESPONSE), 0);
      break;
    }
  }
  Log::getInstance().info(request.getMethod() + " " + request.getHost() +
                          request.getUri());
  return true;
}

bool Client::handleRequest() {
  char buffer[BUFFER_SIZE];

  int bytes_read;
  int status;

  while (true) {
    if (parser.status == HttpRequestParseStatus::EXPECT_CONTINUE) {
      return handleContinue();
    }
    std::string raw = "";
    bytes_read = read(fd, buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
      raw = buffer;
      while (bytes_read == BUFFER_SIZE) {
        raw.append(buffer, bytes_read);
        bytes_read = read(fd, buffer, BUFFER_SIZE);
      }
      std::cout << raw << std::endl;
      parser = HttpRequestParser(raw, fd);
      status = parser.parse();
      if ((status == 200 || status == 201) &&
          parser.status == HttpRequestParseStatus::PARSED) {
        return execute();
      } else if ((status == 200 || status == 201) &&
                 parser.status == HttpRequestParseStatus::EXPECT_CONTINUE) {
        Log::getInstance().debug("Request is to be continued: " +
                                 parser.getHttpRequest().getHost());
        return true;
      }
      auto request = parser.getHttpRequest();
      Log::getInstance().error(
          "Something went wrong while processing request: " + raw);
      response.setStatusCode(status);
      std::string responseString = response.getResponse();
      send(fd, responseString.c_str(), responseString.length(), 0);
      close(fd);
      return false;
    } else if (bytes_read < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      continue;
    } else {
      if (bytes_read < 0) {
        throw net::readFailed(std::strerror(errno));
      }
      close(fd);
      return false;
    }
  }
}
