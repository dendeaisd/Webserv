#include "../../include/networking/PollManager.hpp"

#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../include/networking/PollManagerExceptions.hpp"

using namespace net;

void PollManager::addSocket(int fd, short events) {
  if (fd < 0)
    throw invalidFd(fd);

  for (std::vector<struct pollfd>::const_iterator it = fds_.begin();
       it != fds_.end(); ++it) {
    if (it->fd == fd && it->events == events)
      throw duplicateSocket();
  }

  struct pollfd poll = {fd, events, 0};
  fds_.push_back(poll);
}

void PollManager::removeSocket(int fd) {
  if (fd < 0) {
    throw invalidFd(fd);
  }

  std::cout << "PollManager::removeSocket: Removing socket " << fd << std::endl;

  for (std::vector<struct pollfd>::iterator it = fds_.begin(); it != fds_.end();
       ++it) {
    if (it->fd == fd) {
      std::cout << "PollManager::removeSocket: Found socket " << fd
                << " in list" << std::endl;

      // Check socket state
      int optval;
      socklen_t optlen = sizeof(optval);
      if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &optval, &optlen) == 0) {
        if (optval != 0) {
          std::cerr << "PollManager::removeSocket: Socket " << fd
                    << " has pending error: " << strerror(optval) << std::endl;
        }
      } else {
        std::cerr << "PollManager::removeSocket: Error getting socket error"
                  << std::endl;
      }

      // Check for pending data
      int bytesAvailable;
      if (ioctl(fd, FIONREAD, &bytesAvailable) == 0) {
        if (bytesAvailable > 0) {
          std::cerr << "PollManager::removeSocket: Socket " << fd
                    << " has pending data: " << bytesAvailable << " bytes"
                    << std::endl;
        }
      } else {
        std::cerr
            << "PollManager::removeSocket: Error checking for pending data"
            << std::endl;
      }

      // Close the socket
      close(fd);

      // Set the socket to an invalid value
      it->fd = -1;

      fds_.erase(it);
      std::cout << "PollManager::removeSocket: Socket " << fd
                << " removed successfully" << std::endl;

      return;
    }
  }

  std::cerr << "PollManager::removeSocket: Error: File descriptor " << fd
            << " not found in poll list" << std::endl;
  throw pollManagerException("File descriptor not found in poll list");
}

int PollManager::pollSockets(int timeout) {
  if (fds_.empty()) {
    throw pollManagerException("fds_ vector is empty");
  }

  int numEvents = poll(fds_.data(), fds_.size(), timeout);
  if (numEvents == -1) {
    throw pollFailed(std::strerror(errno));
  } else if (numEvents == 0) {
    std::cout << "PollManager::pollSockets: Timeout" << std::endl;
    return 0; // Return 0 to indicate timeout
  }

  std::vector<int> socketsToRemove;

  // Check for the actual events that have happened
  for (pollfd& pfd : fds_) {
    if (pfd.revents & POLLIN) {
      // Handle POLLIN event (data is ready to be read)
      // Actual data handling logic to be implemented as per server needs
    }
    if (pfd.revents & POLLOUT) {
      // Handle POLLOUT event (ready to send data)
      // Actual data sending logic to be implemented as per server needs
    }
    if (pfd.revents & POLLERR) {
      // POLLERR event (error on the socket)
      socketsToRemove.push_back(pfd.fd);
    }
    if (pfd.revents & POLLHUP) {
      // POLLHUP event (connection hang up)
      socketsToRemove.push_back(pfd.fd);
    }
    if (pfd.revents & POLLNVAL) {
      // POLLNVAL event (invalid request)
      socketsToRemove.push_back(pfd.fd);
    }
  }

  // Remove closed or errored sockets
  for (int fd : socketsToRemove) {
    if (fd >= 0) {
      removeSocket(fd);
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

size_t PollManager::getPollSize() const {
  return fds_.size();
}
