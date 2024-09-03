#ifndef POLLMANAGER_H
#define POLLMANAGER_H

#include <poll.h>

#include <vector>

#include "PollManagerExceptions.hpp"

class PollManager {
 public:
  PollManager();
  void addSocket(int fd, int events = POLLIN);
  void removeSocket(int fd);
  void pollSockets();
  std::vector<struct pollfd>& getFds();

 private:
  std::vector<struct pollfd> _fds;
};

#endif
