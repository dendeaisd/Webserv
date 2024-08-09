#ifndef CGI_HPP
#define CGI_HPP

#include <string>

namespace cgi {
class CGI {
 public:
  CGI(int fd);
  ~CGI();
  void run();
  int send();
  int receive();

 private:
  int pipeInFd_[2];
  int pipeOutFd_[2];
  int pid_;
};
}  // namespace cgi

#endif