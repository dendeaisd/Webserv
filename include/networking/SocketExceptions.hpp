#ifndef SOCKETEXCEPTIONS_H
#define SOCKETEXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace net {

class SocketException : public std::runtime_error {
public:
  SocketException(const std::string &message) : std::runtime_error(message) {}
};

class InvalidAddressException : public SocketException {
public:
  InvalidAddressException(const std::string &address)
      : SocketException("Invalid address: " + address) {}
};

class BindFailedException : public SocketException {
public:
  BindFailedException(const std::string &message)
      : SocketException("Bind failed: " + message) {}
};

} // namespace net

#endif // SOCKET_EXCEPTIONS_H
