#include "../../include/networking/Client.hpp"

#include <fcntl.h>

#include <cerrno>
#include <cstring>
#include <iostream>

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

bool Client::sendDefaultFavicon() {
  response.setStatusCode(200);
  response.setFile("./default/favicon-dt.png", "image/x-icon");
  std::string responseString = response.getResponse();
  send(fd, responseString.c_str(), responseString.length(), 0);
  return true;
}
bool Client::handleRequest() {
  char buffer[BUFFER_SIZE];

  int bytes_read;
  int status;

  while (true) {
    if (parser.status == HttpRequestParseStatus::EXPECT_CONTINUE) {
      status = parser.handshake();
      if (status == 200) {
        if (parser.status == HttpRequestParseStatus::PARSED) {
          send(fd, HTTP_RESPONSE, strlen(HTTP_RESPONSE), 0);
          close(fd);
          Log::getInstance().debug(
              "Successful multipart/octet-stream request with handshake");
        }
        return true;
      } else {
        Log::getInstance().error(
            "Something went wrong while processing request: " +
            parser.getHttpRequest().getHost());
        // TODO: send response and close connection
        close(fd);
        return false;
      }
    }
    bytes_read = read(fd, buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      std::cout << buffer << std::endl;
      parser = HttpRequestParser(buffer, fd);
      status = parser.parse();
      if (status == 200) {
        auto request = parser.getHttpRequest();
        if (request.getHandler() == HttpRequestHandler::CGI) {
          Log::getInstance().debug("Successful request. CGI");
          CGIFileManager cgiFileManager("./cgi-bin");
          CGI cgi(fd, cgiFileManager, request);
          cgi.run();
        } else if (request.getHandler() == HttpRequestHandler::FAVICON) {
          Log::getInstance().debug("Successful request. Favicon");
          sendDefaultFavicon();
        } else {
          send(fd, HTTP_RESPONSE, strlen(HTTP_RESPONSE), 0);
        }
      } else {
        // TODO: send response and close connection
        std::cout << "Status: " << status << std::endl;
        auto request = parser.getHttpRequest();
        std::cout << request.getHost() << std::endl;
        std::cout << "Failed to parse" << std::endl;
      }
      return true;
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
