#include "../../include/cgi/CGI.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <vector>

const char *HTTP_RESPONSE_CGI =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 17\r\n"
    "\r\n"
    "Hello, Universe!\n";

CGI::CGI(int fd, CGIFileManager &cgiFileManager,
              HttpRequest &request) {
  fd_ = fd;
  script_ = "." + request.getUri();
  language_ = cgiFileManager.getExecutor(script_);
  if (language_.empty()) {
    throw std::runtime_error("Failed to get executor for script");
  }
  // load();
  if (pipe(pipeInFd_) == -1 || pipe(pipeOutFd_) == -1) {
    throw std::runtime_error("Failed to create pipe");
  }
}

void CGI::run() {
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
    executeCGI();
    throw std::runtime_error("Failed to execute script");
  } else {
    close(pipeInFd_[0]);
    close(pipeOutFd_[1]);
  }
  wait();
}

void CGI::wait() {
  // TODO: make this non-blocking and check if timeout reached return 408
  // error with timeout message
  int status;
  waitpid(pid_, &status, 0);  // change to WNOHANG to make it non-blocking and
                              // update relevant code
  if (WIFEXITED(status)) {
    if (WEXITSTATUS(status) != 0) {
      // TODO: return 500 error
      throw std::runtime_error("Script exited with non-zero status");
    }
    int sent = send(fd_, HTTP_RESPONSE_CGI, strlen(HTTP_RESPONSE_CGI), 0);
    if (sent < 0) {
      throw std::runtime_error(std::strerror(errno));
    }
    if (sent < static_cast<int>(strlen(HTTP_RESPONSE_CGI))) {
      // TODO: return 500 error
      throw std::runtime_error("Failed to send full response");
    }
  } else {
    // TODO: return 500 error
    throw std::runtime_error("Script did not exit normally");
  }
}

void CGI::executeCGI() {
  std::vector<char *> args;
  std::vector<char *> envp;

  args.push_back(const_cast<char *>(language_.c_str()));
  args.push_back(const_cast<char *>(script_.c_str()));
  args.push_back(nullptr);

  envp.push_back(const_cast<char *>("CONTENT_LENGTH=0"));
  envp.push_back(nullptr);

  execve(language_.c_str(), args.data(), envp.data());
  // Log here cause execve failed if it gives control back
  std::cerr << "Failed to execute script" << std::endl;
  exit(1);
}

int CGI::load() {
  char buffer[4096];
  int bytes_read;
  while (true) {
    bytes_read = read(fd_, buffer, 4096);
    if (bytes_read > 0) {
      request_.append(buffer, bytes_read);
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
  std::cout << request_ << std::endl;
  return 0;
}
