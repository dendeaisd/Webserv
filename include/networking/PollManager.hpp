#ifndef POLLMANAGER_H
#define POLLMANAGER_H

#include <poll.h>

#include <cstring>
#include <vector>

namespace net {

class PollManager {
 public:
  PollManager();
  void addSocket(int fd);
  void removeSocket(int fd);
  void pollSockets();
  std::vector<struct pollfd>& getFds();

 private:
  std::vector<struct pollfd> fds;
};
}  // namespace net

#endif
