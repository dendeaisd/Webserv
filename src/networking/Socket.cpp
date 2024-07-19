#include "../../include/networking/Socket.hpp"
#include "../../include/networking/SocketExceptions.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <unistd.h>

net::Socket::Socket(int domain, int type, int protocol) : fd_(-1), address_() {
  //(void)domain;
  //(void)type;
  //(void)protocol;
  fd_ = socket(domain, type, protocol);
  if (fd_ == -1) {
    std::cerr << "Error creating socket: " << std::strerror(errno) << std::endl;
    throw SocketException(std::strerror(errno));
  }

  address_.sin_family = AF_INET;
  address_.sin_port = htons(8080);
  address_.sin_addr.s_addr = INADDR_ANY;
}

net::Socket::~Socket() {
  if (fd_ != -1) {
    close(fd_);
  }
}

bool net::Socket::Bind(int port, const std::string &address) {
  address_.sin_port = htons(port);

  if (inet_pton(AF_INET, address.c_str(), &address_.sin_addr) <= 0) {
    throw InvalidAddressException(address);
  }

  if (bind(fd_, reinterpret_cast<struct sockaddr *>(&address_),
           sizeof(address_)) < 0) {
    throw BindFailedException(std::strerror(errno));
  }

  return true;
}
