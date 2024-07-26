#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <string>

namespace net {

class Socket {
 public:
  Socket(int domain, int type, int protocol);
  Socket(int fd);
  ~Socket();

  bool Bind(int port, const std::string& address);
  bool Listen(int backlog);
  Socket Accept();
  int getFd() const;
  const sockaddr_in& getClientAddress() const;
  const sockaddr_in& getServerAddress() const;
  static Socket fromFd(int fd);

 private:
  void setNonBlocking(int fd);

  int fd_;
  sockaddr_in address_;
  sockaddr_in clientAddress_;
};

} // namespace net

#endif
