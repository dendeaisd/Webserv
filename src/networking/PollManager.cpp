#include "../../include/networking/PollManager.hpp"
#include "../../include/networking/PollManagerExceptions.hpp"
#include <cerrno>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

using namespace net;

void PollManager::addSocket(int fd, short events) {
  if (fd < 0)
    throw invalidFd(fd);

  for(std::vector<struct pollfd>::iterator it = fds_.begin(); it !=fds_.end(); ++it) {
    if (it->fd == fd && it->events == events)
      throw duplicateSocket();
  }

  struct pollfd poll = {fd, events, 0};
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
    std::cout << "Fds size: " << fds_.size() << std::endl;
   int numEvents = poll(fds_.data(), fds_.size(), timeout);

    if (numEvents == -1) {
        throw pollFailed(std::strerror(errno)); // Throw a pollFailed exception
    } else if (numEvents == 0) {
 std::cout << "here1\n" <<std::endl;
        return 0; // Return 0 to indicate timeout
    }
  std::cout << "here\n" <<std::endl;
    //Check for the actual events that have happened
    for(pollfd& pfd : fds_) {
      std::cout << "pollfd: fd=" << pfd.fd << ", revents=" << pfd.revents << std::endl;
        if(pfd.revents & POLLIN) {
            std::cout << "POLLIN event ready on fd " << pfd.fd << std::endl;
        } else if (pfd.revents & POLLOUT) {
            std::cout << "POLLOUT event ready on fd " << pfd.fd << std::endl;
        } else if (pfd.revents & POLLERR) {
            std::cout << "POLLERR event (error) on fd " << pfd.fd << std::endl;
        }
    }
    return numEvents;
}


struct pollfd& PollManager::getPollFd(int idx) {
  if (idx < 0 || static_cast<size_t>(idx) >= fds_.size()) {
    throw pollManagerException("Index out of range");
  }
  
  return fds_[idx];
}

size_t PollManager::getPollSize() const { return fds_.size(); }
