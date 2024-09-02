#ifndef SOCKETEXCEPTIONS_H
#define SOCKETEXCEPTIONS_H

#include <stdexcept>
#include <string>

class socketException : public std::runtime_error {
 public:
  socketException(const std::string& message) : std::runtime_error(message) {}
};

class bindFailed : public socketException {
 public:
  bindFailed(const std::string& message)
      : socketException("Bind failed: " + message) {}
};

class listenFailed : public socketException {
 public:
  listenFailed(const std::string& message)
      : socketException("Listen failed: " + message) {}
};

class acceptFailed : public socketException {
 public:
  acceptFailed(const std::string& message)
      : socketException("Accept failed: " + message) {}
};

class readFailed : public socketException {
 public:
  readFailed(const std::string& message)
      : socketException("Failed to read data:" + message) {}
};

class sendFailed : public socketException {
 public:
  sendFailed(const std::string& message)
      : socketException("Failed to send data:" + message) {}
};

class getFlagsFailed : public socketException {
 public:
  getFlagsFailed(const std::string& message)
      : socketException("Failed to get falgs for socket: " + message) {}
};

class setNonBlockingModeFailed : public socketException {
 public:
  setNonBlockingModeFailed(const std::string& message)
      : socketException("Failed to set non-blocking mode: " + message) {}
};

#endif
