#include "../../include/networking/Socket.hpp"
#include "../../include/networking/SocketExceptions.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

using namespace net;

Socket::Socket(int domain, int type, int protocol) : fd_(-1), address_() {
  fd_ = socket(domain, type, protocol);

  if (fd_ == -1) {
    std::cerr << "Error creating socket: " << std::strerror(errno) << std::endl;
    throw SocketException(std::strerror(errno));
  }

  address_.sin_family = AF_INET;
  address_.sin_port = htons(8080);
  address_.sin_addr.s_addr = INADDR_ANY;
}

Socket::~Socket() {
  if (fd_ != -1) {
    close(fd_);
  }
}

bool Socket::Bind(int port, const std::string &address) {
  address_.sin_port = htons(port);

  if (inet_pton(AF_INET, address.c_str(), &address_.sin_addr) <= 0) {
    throw InvalidAddress(address);
  }

  if (bind(fd_, reinterpret_cast<struct sockaddr *>(&address_),
           sizeof(address_)) < 0) {
    throw BindFailed(std::strerror(errno));
  }

  return true;
}

bool Socket::Listen(int backlog) {
  if (listen(fd_, backlog) < 0) {
    throw ListenFailed(std::strerror(errno));
  }

  return true;
}

/*Socket Socket::Accept() {
  int newFd = accept(fd_, reinterpret_cast<struct sockaddr *>(&clientAddress_),
                     sizeof(clientAddress_));
  if (newFd < 0)
    throw SomeError;
  return Socket(newFd);
}*/

int Socket::getFd() const { return fd_; }
