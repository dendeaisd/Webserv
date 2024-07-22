#include "../../include/networking/Socket.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>

#include "../../include/networking/SocketExceptions.hpp"

using namespace net;

Socket::Socket(int domain, int type, int protocol) : fd_(-1), address_() {
  fd_ = socket(domain, type, protocol);

  if (fd_ == -1) {
    throw socketException(std::strerror(errno));
  }

  int opt = 1;
  if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
    close(fd_);
    throw socketException(std::strerror(errno));
  }
  address_.sin_family = domain;
}

Socket::Socket(int fd) : fd_(fd), address_(), clientAddress_() {}

Socket::~Socket() {
  if (fd_ != -1) {
    close(fd_);
  }
}

bool Socket::Bind(int port, const std::string &address) {
  address_.sin_port = htons(port);

  if (inet_pton(AF_INET, address.c_str(), &address_.sin_addr) <= 0) {
    throw invalidAddress(address);
  }

  if (bind(fd_, reinterpret_cast<struct sockaddr *>(&address_),
           sizeof(address_)) < 0) {
    throw bindFailed(std::strerror(errno));
  }

  return true;
}

bool Socket::Listen(int backlog) {
  if (listen(fd_, backlog) < 0) {
    throw listenFailed(std::strerror(errno));
  }

  return true;
}

Socket Socket::Accept() {
  socklen_t addrLen = sizeof(clientAddress_); 
  int newFd = accept(fd_, reinterpret_cast<struct sockaddr *>(&clientAddress_),
                    &addrLen);
  if (newFd < 0)
    throw acceptFailed(std::strerror(errno));
  return Socket(newFd);
}

int Socket::getFd() const { return fd_; }

/*const sockaddr_in& Socket::getClientAddress() const {
    return clientAddress_;
}*/

Socket Socket::fromFd(int fd) {
    return Socket(fd);  // This call is now unambiguous due to explicit constructor
}

