#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <sys/socket.h>
#include <sys/types.h>

namespace net {

class Socket {
public:
  explicit Socket(int domain = AF_INET, int type = SOCK_STREAM,
                  int protocol = 0);
  ~Socket();

  bool Bind(int port, const std::string &address);
  bool Listen(int backlog);

private:
  int fd_;
};

} // namespace net

#endif
