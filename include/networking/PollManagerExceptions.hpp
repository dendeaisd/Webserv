#ifndef POLLMANGEREXCEPTIONS_H
#define POLLMANGEREXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace net {

class pollManagerException : public std::runtime_error {
 public:
  pollManagerException(const std::string& message) 
      : std::runtime_error(message) {}
};

class invalidFd : public pollManagerException {
 public:
  invalidFd(int fd) 
      : pollManagerException("Invalid file descriptor: " 
                             + std::to_string(fd)) {}
};

class pollFailed : public pollManagerException {
 public:
  pollFailed(const std::string& message)
      : pollManagerException("Poll failed: " + message) {} 
};

}  // namespace net 
  
#endif
