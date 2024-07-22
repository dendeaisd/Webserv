#ifndef SOCKETEXCEPTIONS_H
#define SOCKETEXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace net {

class socketException : public std::runtime_error {
 public:
  socketException(const std::string &message) : std::runtime_error(message) {}
};

class invalidAddress : public socketException {
 public:
  invalidAddress(const std::string &address)
      : socketException("Invalid address: " + address) {}
};

class bindFailed : public socketException {
 public:
  bindFailed(const std::string &message)
      : socketException("Bind failed: " + message) {}
};

class listenFailed : public socketException {
 public:
  listenFailed(const std::string &message)
      : socketException("Listen failed: " + message) {}
};

class acceptFailed : public socketException {
 public:
  acceptFailed(const std::string &message)
      : socketException("Accept failed: " + message) {}
};

}  // namespace net

#endif
