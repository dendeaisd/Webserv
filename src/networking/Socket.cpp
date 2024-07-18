#include "../../include/networking/Socket.hpp"
#include <iostream>
#include <unistd.h>

net::Socket::Socket(int domain, int type, int protocol) : fd_(-1) {
  fd_ = socket(domain, type, protocol);
  if (fd_ == -1) {
    std::cerr << "Error creating socket: " << std::strerror(errno) << std::endl;
    throw std::runtime_error(std::strerror(errno));
  }
}

net::Socket::~Socket() {
  if (fd_ != -1) {
    close(fd_);
  }
}
