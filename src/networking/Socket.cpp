#include "../../include/networking/Socket.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

#include "../../include/log/Log.hpp"

Socket::Socket(int domain, int type, int protocol) {
  if ((sockFd_ = socket(domain, type, protocol)) == -1) {
    Log::getInstance().error("Failed to create socket");
    throw socketException("Failed to create socket");
  }
}

Socket::~Socket() { close(sockFd_); }

void Socket::setSocketOption(int level, int optname, int optval) {
  if (setsockopt(sockFd_, level, optname, (char *)&optval, sizeof(optval)) <
      0) {
    Log::getInstance().error("Setsockopt failed");
    throw socketException("Setsockopt failed");
  }
}

void Socket::bindSocket(int port) {
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(sockFd_, (struct sockaddr *)&address, sizeof(address)) < 0) {
    Log::getInstance().error("Failed to bind socket");
    throw bindFailed(std::strerror(errno));
  }
}

void Socket::listenSocket(int backlog) {
  if (listen(sockFd_, backlog) < 0) {
    Log::getInstance().error("Failed to listen on socket");
    throw listenFailed(std::strerror(errno));
  }
}

int Socket::acceptConnection(struct sockaddr_in *address, socklen_t *addrlen) {
  int new_socket;
  if ((new_socket = accept(sockFd_, (struct sockaddr *)address, addrlen)) < 0) {
    Log::getInstance().error("Failed to accept connection");
    throw acceptFailed(std::strerror(errno));
  }
  return new_socket;
}

int Socket::readData(int sockFd_, char *buffer, size_t size) {
  int bytes_read = read(sockFd_, buffer, size);
  if (bytes_read < 0) {
    Log::getInstance().error("Failed to read from socket");
    throw readFailed(std::strerror(errno));
  }
  return bytes_read;
}

int Socket::sendData(int sockFd_, const char *buffer, size_t size) {
  int bytes_sent = send(sockFd_, buffer, size, 0);
  if (bytes_sent < 0) {
    throw sendFailed(std::strerror(errno));
  }
  return bytes_sent;
}

void Socket::setNonBlocking() {
  int flags = fcntl(sockFd_, F_GETFL, 0);
  if (flags < 0) {
    Log::getInstance().error("Failed to get flags");
    throw getFlagsFailed(std::strerror(errno));
  }
  if (fcntl(sockFd_, F_SETFL, flags | O_NONBLOCK) < 0) {
    Log::getInstance().error("Failed to set non-blocking mode");
    throw setNonBlockingModeFailed(std::strerror(errno));
  }
}

int Socket::getSocketFd() const { return sockFd_; }
