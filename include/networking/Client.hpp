#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <memory>

#include "../../include/Event.hpp"
#include "../../include/request/HttpRequestParser.hpp"
#include "../../include/response/HttpResponse.hpp"
#include "SocketExceptions.hpp"

class Client {
 public:
  Client(int fd);
  ~Client();

  int getFd() const;
  bool handleRequest();

 private:
  int _fd;
  HttpRequestParser _parser;
  HttpResponse _response;
  std::unique_ptr<Event> _events;

  bool sendDefaultFavicon();
  bool sendDefaultPage();
  bool sendDirectoryListings(const std::string& path);

  std::string generateDirectoryListing(const std::string& path,
                                       const std::string& requestUri,
                                       const std::string& inject);
  bool handleContinue();
  bool execute();
};

#endif  // CLIENT_H
