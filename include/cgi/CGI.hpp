#ifndef CGI_HPP
#define CGI_HPP

#include <string>

#include "../request/HttpRequest.hpp"
#include "CGIFileManager.hpp"

namespace cgi {
class CGI {
 public:
  CGI(int fd, cgi::CGIFileManager &cgiFileManager, HttpRequest &request);
  ~CGI() {}
  void run();
  void wait();
  void executeCGI();
  int load();

 private:
  int fd_;
  int pipeInFd_[2];
  int pipeOutFd_[2];
  int pid_;
  std::string response_;
  std::string request_;
  std::string script_;
  std::string language_;
};
}  // namespace cgi

#endif