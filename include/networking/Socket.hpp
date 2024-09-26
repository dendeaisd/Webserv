#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>

#include "SocketExceptions.hpp"

class Socket {
 public:
  Socket(int domain, int type, int protocol);
  ~Socket();

  void bindSocket(int port, const std::string &address = "");
  void listenSocket(int backlog);
  int acceptConnection(struct sockaddr_in *address, socklen_t *addrlen);
  void setSocketOption(int level, int optname, int optval);
  //   int readData(int sockfd, char *buffer, size_t size);
  //   int sendData(int sockfd, const char *buffer, size_t size);
  void setNonBlocking();
  int getSocketFd() const;

 private:
  int _sockFd;
};

#endif
