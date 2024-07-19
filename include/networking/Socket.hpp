#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
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
  Socket Accept();

  int getFd() const;

private:
  int fd_;
  struct sockaddr_in address_;       // Server addess
  struct sockaddr_in clientAddress_; // Client address
};

} // namespace net

#endif
