#ifndef CGI_HPP
#define CGI_HPP

#include <cstring>
#include <string>

#include "../request/HttpRequest.hpp"
#include "CGIFileManager.hpp"

class CGI {
 public:
  CGI(int fd, HttpRequest &request);
  ~CGI();
  bool run();
  bool wait();

 private:
  int _fd;
  int _pipeInFd[2];
  int _pipeOutFd[2];
  int _pid;
  bool _unableToExecute;
  std::string _response;
  std::string _stream;
  std::string _script;
  std::string _language;
  std::string _urlArg;
  HttpRequest _request;
  CGI();
  void executeCGI();
  void killChild();
  bool handleTimeout();
  bool handleError(std::string logMessage);
  bool tunnelData();
};

#endif