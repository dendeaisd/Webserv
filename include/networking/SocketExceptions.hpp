#ifndef SOCKETEXCEPTIONS_H
#define SOCKETEXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace net {

class SocketException : public std::runtime_error {
public:
  SocketException(const std::string &message) : std::runtime_error(message) {}
};

class InvalidAddress : public SocketException {
public:
  InvalidAddress(const std::string &address)
      : SocketException("Invalid address: " + address) {}
};

class BindFailed : public SocketException {
public:
  BindFailed(const std::string &message)
      : SocketException("Bind failed: " + message) {}
};

class ListenFailed : public SocketException {
public:
  ListenFailed(const std::string &message)
      : SocketException("Listen failed: " + message) {}
};

} // namespace net

#endif
