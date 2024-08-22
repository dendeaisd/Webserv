#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <unistd.h>

#include "../../include/request/HttpRequestParser.hpp"
#include "../../include/response/HttpResponse.hpp"
#include "SocketExceptions.hpp"

class Client {
 public:
  Client(int fd);
  ~Client();

  int getFd() const;
  bool handleRequest();
  bool sendDefaultFavicon();

 private:
  int fd;
  HttpRequestParser parser;
  HttpResponse response;
};
#endif  // CLIENT_H
