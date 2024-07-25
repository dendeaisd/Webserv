#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

class Client {
 public:
  Client(int fd) : fd_(fd) {}

  int getFd() const { return fd_; }

  bool handleData() {
    char buffer[1024];
    ssize_t bytesRead = recv(fd_, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
      // Connection closed or error
      std::cerr << "Client disconnected or error: " << strerror(errno)
                << std::endl;
      close(fd_);
      fd_ = -1;
      return false;
    } else {
      buffer[bytesRead] = '\0';
      std::cout << "Received from client " << fd_ << ": " << buffer
                << std::endl;
      // Echo the received data back to the client
      std::string response =
          "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(bytesRead) +
          "\r\n\r\n" + std::string(buffer);
      send(fd_, response.c_str(), response.size(), 0);
      return true;
    }
  }

 private:
  int fd_;
};

#endif  // CLIENT_H
