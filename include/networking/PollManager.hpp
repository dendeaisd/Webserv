#ifndef POLLMANAGER_H
#define POLLMANAGER_H

#include <vector>
#include <poll.h>

namespace net {

class PollManager {
 public:
  void addSocket(int fd);
  void removeSocket(int fd);
  int pollSockets(int timeout);
  
  struct pollfd& getPollFd(int idx);
  size_t getPollSize() const;
 private:
  std::vector<struct pollfd> fds_;
};

}  // namespace net

#endif
