#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <unistd.h>

class Client {
 public:
  Client(int fd);
  ~Client();

  int getFd() const;
  bool handleRequest();

 private:
  int fd;
};
#endif  // CLIENT_H
