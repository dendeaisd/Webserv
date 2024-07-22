#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>

namespace net {

class Socket {
 public:
  explicit Socket(int domain, int type, int protocol);
  ~Socket();

  static Socket fromFd(int fd);
  bool Bind(int port, const std::string &address);
  bool Listen(int backlog);
  Socket Accept();

  int getFd() const;
  const sockaddr_in& getClientAddress() const; 

 private:
  explicit Socket(int fd);
  int fd_;
  struct sockaddr_in address_;
  struct sockaddr_in clientAddress_; 
  
};

}  // namespace net

#endif
