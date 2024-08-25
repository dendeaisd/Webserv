#include "../../include/cgi/CGI.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <ctime>
#include <iostream>
#include <vector>

#include "../../include/log/Log.hpp"
#define BUFFER_SIZE 4096
#define TIMEOUT 20

CGI::CGI(int fd, CGIFileManager &cgiFileManager, HttpRequest &request) {
  fd_ = fd;
  _request = request;
  _script = "." + request.getUri();
  _language = cgiFileManager.getExecutor(_script);
  if (_language.empty()) {
    Log::getInstance().error("Failed to get executor for script");
  }
  // load();
  if (pipe(pipeInFd_) == -1 || pipe(pipeOutFd_) == -1) {
    Log::getInstance().error("Failed to create pipe");
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
  int status;

  int flags = fcntl(pipeOutFd_[0], F_GETFL, 0);
  fcntl(pipeOutFd_[0], F_SETFL, flags | O_NONBLOCK);
  while (true) {
    if (_request.checkTimeout()) {
      // return request timed out
      Log::getInstance().error("Request timed out");
      // TODO: return request timed out error code 408
      // TODO: kill child process
      // TODO: Move all send calls to client class
      break;
    }
    pid_t result = waitpid(pid_, &status, WNOHANG);
    if (result == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        usleep(50000);
        continue;
      }
      Log::getInstance().error("Failed to wait for child process");
      break;
    }
    if (result == 0) {
      usleep(50000);
      continue;
    }
    if (result == pid_) {
      if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
          // TODO: return 500 error
          Log::getInstance().error("Script exited with non-zero status");
          break;
        }
      } else {
        // TODO: return 500 error
        Log::getInstance().error("Script did not exit normally");
        break;
      }
    }
    char buffer[BUFFER_SIZE + 1];
    int bytes_read;
    bytes_read = read(pipeOutFd_[0], buffer, BUFFER_SIZE);
    Log::getInstance().info("Read " + std::to_string(bytes_read) + " bytes");
    if (bytes_read > 0) {
      Log::getInstance().info("Buffer: " + std::string(buffer));
      int sent = send(fd_, buffer, bytes_read, 0);
      Log::getInstance().info("Sent " + std::to_string(sent) + " bytes");
      if (sent < 0) {
        throw std::runtime_error(std::strerror(errno));
      }
      break;
    } else if (bytes_read < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      usleep(50000);
      continue;
    } else {
      if (bytes_read < 0) {
        throw std::runtime_error(std::strerror(errno));
      }
      break;
    }
  }
  close(pipeOutFd_[0]);
}

void CGI::executeCGI() {
  std::vector<char *> args;
  std::vector<char *> envp;

  args.push_back(const_cast<char *>(_language.c_str()));
  args.push_back(const_cast<char *>(_script.c_str()));
  args.push_back(nullptr);

  envp.push_back(const_cast<char *>("CONTENT_LENGTH=0"));
  envp.push_back(nullptr);

  execve(_language.c_str(), args.data(), envp.data());
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
      _stream.append(buffer, bytes_read);
      if (_stream.find("\r\n\r\n") != std::string::npos) {
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
  std::cout << _stream << std::endl;
  return 0;
}
