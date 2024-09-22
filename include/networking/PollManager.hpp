#ifndef POLLMANAGER_H
#define POLLMANAGER_H

#include <poll.h>

#include <vector>
#include <unordered_map>

#include "PollManagerExceptions.hpp"

class PollManager {
 public:
  PollManager();
  void addSocket(int fd, int events = POLLIN, int port = -1);
  void removeSocket(int fd);
  void pollSockets();
  std::vector<struct pollfd>& getFds();
  std::unordered_map<int, int> _fdToPortMap;

 private:
  std::vector<struct pollfd> _fds;
};

#endif
