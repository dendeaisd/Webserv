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
  if ((_sockFd = socket(domain, type, protocol)) == -1) {
    Log::getInstance().error("Failed to create socket");
    throw socketException("Failed to create socket");
  }
}

Socket::~Socket() { close(_sockFd); }

void Socket::setSocketOption(int level, int optname, int optval) {
  if (setsockopt(_sockFd, level, optname, (char *)&optval, sizeof(optval)) <
      0) {
    Log::getInstance().error("Setsockopt failed");
    throw socketException("Setsockopt failed");
  }
}

void Socket::bindSocket(int port, const std::string &address) {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (address.empty()) {
    addr.sin_addr.s_addr = INADDR_ANY;
  } else {
    if (inet_pton(AF_INET, address.c_str(), &addr.sin_addr) <= 0) {
      Log::getInstance().error("inet_pton failed: " + address);
      throw bindFailed("Invalid address: " + address);
    }
  }
  if (bind(_sockFd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    Log::getInstance().error("Failed to bind socket to " + address + ":" +
                             std::to_string(port));
    throw bindFailed(std::strerror(errno));
  }

  // TODO: rm this, only for verification
  struct sockaddr_in boundAddr;
  socklen_t addrLen = sizeof(boundAddr);
  if (getsockname(_sockFd, (struct sockaddr *)&boundAddr, &addrLen) == 0) {
    char boundAddrStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &boundAddr.sin_addr, boundAddrStr, INET_ADDRSTRLEN);
    int boundPort = ntohs(boundAddr.sin_port);
    Log::getInstance().info("Socket successfully bound to " +
                            std::string(boundAddrStr) + ":" +
                            std::to_string(boundPort));
  } else {
    Log::getInstance().error("Failed to retrieve bound address: " +
                             std::string(std::strerror(errno)));
  }
}

void Socket::listenSocket(int backlog) {
  if (listen(_sockFd, backlog) < 0) {
    Log::getInstance().error("Failed to listen on socket");
    throw listenFailed(std::strerror(errno));
  }
}

int Socket::acceptConnection(struct sockaddr_in *address, socklen_t *addrlen) {
  int new_socket;
  if ((new_socket = accept(_sockFd, (struct sockaddr *)address, addrlen)) < 0) {
    Log::getInstance().error("Failed to accept connection");
    throw acceptFailed(std::strerror(errno));
  }
  return new_socket;
}

int Socket::readData(int _sockFd, char *buffer, size_t size) {
  int bytes_read = read(_sockFd, buffer, size);
  if (bytes_read < 0) {
    Log::getInstance().error("Failed to read from socket");
    throw readFailed(std::strerror(errno));
  }
  return bytes_read;
}

int Socket::sendData(int _sockFd, const char *buffer, size_t size) {
  int bytes_sent = send(_sockFd, buffer, size, 0);
  if (bytes_sent < 0) {
    throw sendFailed(std::strerror(errno));
  }
  return bytes_sent;
}

void Socket::setNonBlocking() {
  int flags = fcntl(_sockFd, F_GETFL, 0);
  if (flags < 0) {
    Log::getInstance().error("Failed to get flags");
    throw getFlagsFailed(std::strerror(errno));
  }
  if (fcntl(_sockFd, F_SETFL, flags | O_NONBLOCK) < 0) {
    Log::getInstance().error("Failed to set non-blocking mode");
    throw setNonBlockingModeFailed(std::strerror(errno));
  }
}

int Socket::getSocketFd() const { return _sockFd; }
