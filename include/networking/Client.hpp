#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>

class Client {
 public:
  Client(int fd) : fd_(fd) {}

  int getFd() const { return fd_; }

  bool handleData() {
    char buffer[1024];
    ssize_t bytesRead = recv(fd_, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
      buffer[bytesRead] = '\0';
      std::string response = "HTTP/1.1 200 OK\r\nContent-Length: ";
      response += std::to_string(bytesRead) + "\r\n\r\n" + buffer;
      send(fd_, response.c_str(), response.size(), 0);
    } else {
      // handle error or close
      close(fd_);
      return false;
    }
    return true;
  }

 private:
  int fd_;
};

#endif  // CLIENT_H
