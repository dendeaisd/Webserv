#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>

class Client {
 public:
  Client(int fd);
  ~Client();

  int getFd() const;
  void handleRequest();

 private:
  int fd;
};
#endif  // CLIENT_H
