#ifndef CGI_HPP
#define CGI_HPP

#include <fcntl.h>
#include <unistd.h>

#include <string>

namespace cgi {
class CGI {
 public:
  CGI(int fd) {
    fd_ = fd;
    load();
    if (pipe(pipeInFd_) == -1 || pipe(pipeOutFd_) == -1) {
      throw std::runtime_error("Failed to create pipe");
    }
  }
  ~CGI() {}
  void run() {
    pid_ = fork();
    if (pid_ == -1) {
      throw std::runtime_error("Failed to fork");
    }
    if (pid_ == 0) {
      close(pipeInFd_[1]);
      close(pipeOutFd_[0]);
      dup2(pipeInFd_[0], STDIN_FILENO);
      dup2(pipeOutFd_[1], STDOUT_FILENO);
      close(pipeInFd_[0]);
      close(pipeOutFd_[1]);
      execlp(language_.c_str(), language_.c_str(), script_.c_str(), NULL);
      throw std::runtime_error("Failed to execute script");
    } else {
      close(pipeInFd_[0]);
      close(pipeOutFd_[1]);
    }
  }
  int send();
  int receive();
  int load() {
    char buffer[4096];
    int bytes_read;
    while (true) {
      bytes_read = read(fd_, buffer, 4096);
      if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        request_ += buffer;
        if (request_.find("\r\n\r\n") != std::string::npos) {
          break;
        }
      } else if (bytes_read < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        continue;
      } else {
        if (bytes_read < 0) {
          throw std::runtime_error(std::strerror(errno));
        }
        close(fd_);
        return -1;
      }
    }
    return 0;
  }

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