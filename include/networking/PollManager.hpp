#ifndef POLLMANAGER_H
#define POLLMANAGER_H

#include <poll.h>

#include <cstring>
#include <vector>

namespace net {

class PollManager {
 public:
  void addSocket(int fd, short events = POLLIN);
  void removeSocket(int fd);
  int pollSockets(int timeout);
  struct pollfd& getPollFd(int idx);
  std::size_t getPollSize() const;

 private:
  std::vector<struct pollfd> fds_;
};

} // namespace net

#endif
