#include "../../include/networking/Client.hpp"

#include <fcntl.h>

#include <cerrno>
#include <cstring>
#include <iostream>

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

  while (true) {
    bytes_read = read(fd, buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      std::cout << "Received: " << buffer << std::endl;
      send(fd, HTTP_RESPONSE, strlen(HTTP_RESPONSE), 0);
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