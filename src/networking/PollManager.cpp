#include "../../include/networking/PollManager.hpp"
#include "../../include/networking/PollManagerExceptions.hpp"
#include <cerrno>
#include <algorithm>
#include <cstddef>
#include <vector>

using namespace net;

void PollManager::addSocket(int fd) {
  if (fd < 0)
    throw invalidFd(fd);
  
  struct pollfd poll = {fd, POLLIN, 0};
  fds_.push_back(poll);
}

void PollManager::removeSocket(int fd) {
  if (fd < 0)
    throw invalidFd(fd);
  
  for (std::vector<struct pollfd>::iterator it = fds_.begin();
                                      it != fds_.end(); ++it) {
    if (it->fd == fd) {
      std::iter_swap(it, fds_.end() - 1);
      fds_.pop_back();
      return;
    }
  }

  throw pollManagerException("File descriptor not found in poll list"); 
}

int PollManager::pollSockets(int timeout) {
  if (fds_.empty()) {
    throw pollManagerException("fds_ vector is empty");
  }
  int returnVal = poll(&fds_[0], fds_.size(), timeout);

  if (returnVal == 0) {
    return 0; // timeout happend
  }

  if (returnVal == -1) {
    throw pollFailed(std::strerror(errno)); 
  }
  return returnVal;
}

struct pollfd& PollManager::getPollFd(int idx) {
  if (idx < 0 || static_cast<size_t>(idx) >= fds_.size()) {
    throw pollManagerException("Index out of range");
  }
  
  return fds_[idx];
}

size_t PollManager::getPollSize() const { return fds_.size(); }
