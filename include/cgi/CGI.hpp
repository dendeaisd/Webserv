#ifndef CGI_HPP
#define CGI_HPP

#include <cstring>
#include <string>

#include "../request/HttpRequest.hpp"
#include "CGIFileManager.hpp"

class CGI {
 public:
  CGI(int fd, HttpRequest &request);
  ~CGI() {}
  bool run();
  bool wait();
  int load();

 private:
  int fd_;
  int pipeInFd_[2];
  int pipeOutFd_[2];
  int pid_;
  bool _unableToExecute;
  std::string _response;
  std::string _stream;
  std::string _script;
  std::string _language;
  HttpRequest _request;

  void executeCGI();
  void killChild();
  bool handleTimeout();
  bool handleError(std::string logMessage);
  bool tunnelData();
};

#endif