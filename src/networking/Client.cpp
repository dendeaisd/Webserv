#include "../../include/networking/Client.hpp"

#include <fcntl.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include "../../include/cgi/CGI.hpp"
#include "../../include/cgi/CGIFileManager.hpp"

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
        }
        return true;
      } else {
        std::cout << "Handshake failed" << std::endl;
        std::cout << "Status: " << status << std::endl;
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
        std::cout << "Parsed successfully" << std::endl;
        auto request = parser.getHttpRequest();
        if (request.getHandler() == HttpRequestHandler::CGI) {
          std::cout << "CGI" << std::endl;
          CGIFileManager cgiFileManager("./cgi-bin");
          CGI cgi(fd, cgiFileManager, request);
          cgi.run();
        } else {
          std::cout << "STATIC" << std::endl;
          send(fd, HTTP_RESPONSE, strlen(HTTP_RESPONSE), 0);
        }
      } else {
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
