#include "../../include/networking/Socket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>

#include "../../include/networking/SocketExceptions.hpp"

using namespace net;

Socket::Socket(int domain, int type, int protocol) : address_{} {
  fd_ = socket(domain, type, protocol);
  if (fd_ == -1) {
    throw socketException("Failed to create socket");
  }
  address_.sin_family = AF_INET;
  address_.sin_port = 0;                 // Default port
  address_.sin_addr.s_addr = INADDR_ANY; // Default address
  setNonBlocking(fd_);
}

Socket::Socket(int fd) : fd_(fd), address_{} {
  address_.sin_family = AF_INET;
  address_.sin_port = 0;                 // Default port
  address_.sin_addr.s_addr = INADDR_ANY; // Default address
  setNonBlocking(fd_);
};

Socket::~Socket() {
  close(fd_);
}

bool Socket::Bind(int port, const std::string& address) {
  address_.sin_family = AF_INET;
  address_.sin_port = htons(port);
  address_.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(fd_, (struct sockaddr*)&address_, sizeof(address_)) == -1) {
    throw bindFailed("Failed to bind socket");
  }
  return true;
}

bool Socket::Listen(int backlog) {
  if (listen(fd_, backlog) == -1) {
    throw listenFailed("Failed to listen on socket");
  }
  return true;
}

Socket Socket::Accept() {
  sockaddr_in clientAddress;
  socklen_t clientAddressLength = sizeof(clientAddress);
  int clientFd =
      accept(fd_, (struct sockaddr*)&clientAddress, &clientAddressLength);
  if (clientFd == -1) {
    throw acceptFailed("Failed to accept connection");
  }
  Socket clientSocket(clientFd);
  clientSocket.clientAddress_ = clientAddress;
  return clientSocket;
}

int Socket::getFd() const {
  return fd_;
}

const sockaddr_in& Socket::getClientAddress() const {
  return clientAddress_;
}

const sockaddr_in& Socket::getServerAddress() const {
  return address_;
}

Socket Socket::fromFd(int fd) {
  return Socket(fd);
}

void Socket::setNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    throw getFlagsFailed("Failed to get socket flags");
  }
  flags |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) == -1) {
    throw setNonBlockingModeFailed("Failed to set non-blocking mode");
  }
}
