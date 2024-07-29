#include "../../include/networking/PollManager.hpp"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <vector>


using namespace net;

PollManager::PollManager() {}

void PollManager::addSocket(int fd, int events) {
  struct pollfd pfd;
  pfd.fd = fd;
  pfd.events = events;
  pfd.revents = 0;
  fds_.push_back(pfd);
}

void PollManager::removeSocket(int fd) {
  for (size_t i = 0; i < fds_.size(); ++i) {
    if (fds_[i].fd == fd) {
      std::swap(fds_[i], fds_.back());
      fds_.pop_back();
      break;
    }
  }
}

void PollManager::pollSockets() {
  int poll_count = poll(&fds_[0], fds_.size(), -1);
  if (poll_count < 0) {
    perror("poll error");
    exit(EXIT_FAILURE);
  }
}

std::vector<struct pollfd>& PollManager::getFds() { return fds_; }
