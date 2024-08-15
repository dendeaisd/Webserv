#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <unistd.h>

#include "../../include/request/HttpRequestParser.hpp"
#include "SocketExceptions.hpp"

class Client {
 public:
  Client(int fd);
  ~Client();

  int getFd() const;
  bool handleRequest();

 private:
  int fd;
  HttpRequestParser parser;
};
#endif  // CLIENT_H
