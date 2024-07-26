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

  int getFd() const {
    return fd_;
  }

  bool handleData() {
    char buffer[1024];
    ssize_t bytesRead = recv(fd_, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
      buffer[bytesRead] = '\0';
      std::string response = generateResponse(buffer, bytesRead);
      send(fd_, response.c_str(), response.size(), 0);
    } else if (bytesRead == 0) {
      // Client closed the connection
      std::cerr << "Client closed connection: " << fd_ << std::endl;
      close(fd_);
      return false;
    } else {
      // Handle recv() errors
      int err = errno;
      if (err == EAGAIN || err == EINTR) {
        // Handle temporary errors, e.g., retry or wait
        std::cerr << "recv() error: " << strerror(err) << std::endl;
        return true; // Retry or wait
      } else {
        // Handle fatal errors
        std::cerr << "recv() error: " << strerror(err) << std::endl;
        close(fd_);
        return false;
      }
    }
    return true;
  };
  ssize_t recvData();

 private:
  int fd_;

  std::string generateResponse(const char* data, ssize_t dataSize) {
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: ";
    response += std::to_string(dataSize) + "\r\n\r\n";
    response += std::string(data, dataSize);
    return response;
  }
};

#endif // CLIENT_H
