#include "../../include/networking/Socket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>

#include "../../include/networking/SocketExceptions.hpp"

using namespace net;

Socket::Socket(int domain, int type, int protocol) {
  if ((sockFd_ = socket(domain, type, protocol)) == -1) {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }
}

Socket::~Socket() { close(sockFd_); }

void Socket::setSocketOption(int level, int optname, int optval) {
  if (setsockopt(sockFd_, level, optname, (char *)&optval, sizeof(optval)) <
      0) {
    perror("setsockopt");
    close(sockFd_);
    exit(EXIT_FAILURE);
  }
}

void Socket::bindSocket(int port) {
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(sockFd_, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    close(sockFd_);
    exit(EXIT_FAILURE);
  }
}

void Socket::listenSocket(int backlog) {
  if (listen(sockFd_, backlog) < 0) {
    perror("listen");
    close(sockFd_);
    exit(EXIT_FAILURE);
  }
}

int Socket::acceptConnection(struct sockaddr_in *address, socklen_t *addrlen) {
  int new_socket;
  if ((new_socket = accept(sockFd_, (struct sockaddr *)address, addrlen)) < 0) {
    perror("accept");
  }
  return new_socket;
}

int Socket::readData(int sockFd_, char *buffer, size_t size) {
  int bytes_read = read(sockFd_, buffer, size);
  if (bytes_read < 0) {
    perror("read");
  }
  return bytes_read;
}

int Socket::sendData(int sockFd_, const char *buffer, size_t size) {
  int bytes_sent = send(sockFd_, buffer, size, 0);
  if (bytes_sent < 0) {
    perror("send");
  }
  return bytes_sent;
}

void Socket::setNonBlocking() {
  int flags = fcntl(sockFd_, F_GETFL, 0);
  if (flags < 0) {
    perror("fcntl(F_GETFL)");
    exit(EXIT_FAILURE);
  }
  if (fcntl(sockFd_, F_SETFL, flags | O_NONBLOCK) < 0) {
    perror("fcntl(F_SETFL)");
    exit(EXIT_FAILURE);
  }
}

int Socket::getSocketFd() const { return sockFd_; }