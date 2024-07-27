#include "../../include/networking/Client.hpp"

#include <fcntl.h>

#define BUFFER_SIZE 1024

Client::Client(int fd) : fd(fd) { fcntl(fd, F_SETFL, O_NONBLOCK); }

Client::~Client() { close(fd); }

int Client::getFd() const { return fd; }

const char* HTTP_RESPONSE =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "Hello, World!";

void Client::handleRequest() {
  char buffer[BUFFER_SIZE];
  int bytes_read;

  while (true) {
    bytes_read = read(fd, buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      std::cout << "Received: " << buffer << std::endl;
      send(fd, HTTP_RESPONSE, strlen(HTTP_RESPONSE), 0);
    } else if (bytes_read < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      // No data available right now, continue and try again
      continue;
    } else {
      // Error or connection closed
      if (bytes_read < 0) {
        perror("read");
      }
      close(fd);
      fd = -1;
      break;
    }
  }
}