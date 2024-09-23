#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <memory>

#include "../../include/Event.hpp"
#include "../../include/request/HttpRequestParser.hpp"
#include "../../include/response/HttpResponse.hpp"
#include "ServerContext.hpp"
#include "SocketExceptions.hpp"

class Client {
 public:
  Client(int fd, std::shared_ptr<ServerContext> context);
  ~Client();

  int getFd() const;
  bool handleRequest();

  bool isReadyForResponse();
  bool isReadyForRequest();
  bool shouldSendContinue();
  void setReadyForResponse(bool ready);
  bool execute();

 private:
  int _fd;
  std::shared_ptr<ServerContext> _context;
  HttpRequestParser _parser;
  HttpResponse _response;
  std::unique_ptr<Event> _events;
  bool _shouldSendContinue;
  bool _isReadyForResponse;
  bool _isReadyForRequest;

  bool sendDefaultFavicon();
  bool sendWebDocument();
  bool sendDirectoryListings(const std::string& path);

  std::string generateDirectoryListing(const std::string& path,
                                       const std::string& requestUri,
                                       const std::string& inject);
  bool handleContinue();
  bool handleRedirect();
  void reset();
};

#endif  // CLIENT_H
