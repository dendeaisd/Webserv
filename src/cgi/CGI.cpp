#include "../../include/cgi/CGI.hpp"

#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include "../../include/log/Log.hpp"
#include "../../include/response/HttpResponse.hpp"

#define BUFFER_SIZE 4096
#define TIMEOUT 20

CGI::CGI(int fd, CGIFileManager &cgiFileManager, HttpRequest &request) {
  fd_ = fd;
  _request = request;
  _unableToExecute = false;
  _script = "." + request.getUri();
  _language = cgiFileManager.getExecutor(_script);
  if (_language.empty()) {
    Log::getInstance().error("Failed to get executor for script");
    _unableToExecute = true;
  }
  // load();
  if (pipe(pipeInFd_) == -1 || pipe(pipeOutFd_) == -1) {
    Log::getInstance().error("Failed to create pipe");
    _unableToExecute = true;
  }
}

void sendTimeoutResponse(int fd) {
  HttpResponse response(408);
  response.setBody(
      "Request timed out, we apologize for our slowness and unoptimized code.");
  std::string resp = response.getResponse();
  send(fd, resp.c_str(), resp.length(), 0);
}

void sendInternalErrorResponse(int fd) {
  HttpResponse response(500);
  std::string resp = response.getResponse();
  send(fd, resp.c_str(), resp.length(), 0);
}

void CGI::run() {
  if (_unableToExecute) {
    sendInternalErrorResponse(fd_);
    return;
  }
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
  //   wait();
}

void CGI::killChild() {
  if (pid_ != 0) {
    kill(pid_, SIGKILL);
  }
}

/*
This function returns false if the child process is still running and true if
the child process has exited. This return value will be used to decide if we
should remove the event from the event manager.
*/
bool CGI::wait() {
  int status;

  if (_unableToExecute) {
    return true;
  }
  int flags = fcntl(pipeOutFd_[0], F_GETFL, 0);
  fcntl(pipeOutFd_[0], F_SETFL, flags | O_NONBLOCK);
  while (true) {
    if (_request.checkTimeout()) {
      // return request timed out
      Log::getInstance().error("Request timed out");
      // TODO: return request timed out error code 408
      close(pipeOutFd_[0]);
      // TODO: kill child process
      killChild();
      sendTimeoutResponse(fd_);
      // TODO: Move all send calls to client class
      return true;
    }
    pid_t result = waitpid(pid_, &status, WNOHANG);
    if (result == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // still needs time to respond
        return false;
      }
      Log::getInstance().error("Failed to wait for child process");
      close(pipeOutFd_[0]);
      sendInternalErrorResponse(fd_);
      return true;
    }
    if (result == 0) {
      return false;
    }
    if (result == pid_) {
      if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
          // TODO: return 500 error
          Log::getInstance().error("Script exited with non-zero status");
          close(pipeOutFd_[0]);
          sendInternalErrorResponse(fd_);
          return true;
        }
      } else {
        // TODO: return 500 error
        Log::getInstance().error("Script did not exit normally");
        close(pipeOutFd_[0]);
        sendInternalErrorResponse(fd_);
        return true;
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
      close(pipeOutFd_[0]);
      return true;
    } else if (bytes_read < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      return false;
    } else {
      if (bytes_read < 0) {
        Log::getInstance().error("Failed to read from pipe with error: " +
                                 std::string(std::strerror(errno)));
      }
      close(pipeOutFd_[0]);
      return true;
    }
  }
  return false;
}

void CGI::executeCGI() {
  std::vector<char *> args;
  std::vector<char *> envp;
  std::vector<std::string> env_strs;

  args.push_back(const_cast<char *>(_language.c_str()));
  args.push_back(const_cast<char *>(_script.c_str()));
  args.push_back(nullptr);

  envp.push_back(const_cast<char *>(
      std::string("CONTENT_LENGTH=" + _request.getHeader("Content-Length"))
          .c_str()));
  envp.push_back(const_cast<char *>(
      std::string("REQUEST_METHOD=" + _request.getMethod()).c_str()));
  envp.push_back(
      const_cast<char *>(std::string("HOST=" + _request.getHost()).c_str()));
  auto qp = _request.getQueryParams();
  Log::getInstance().info("Query params size: " + std::to_string(qp.size()));
  for (auto it = qp.begin(); it != qp.end(); ++it) {
    env_strs.push_back("QS_" + it->first + "=" + it->second);
  }

  for (auto it = env_strs.begin(); it != env_strs.end(); ++it) {
    Log::getInstance().info("Adding env: " + *it);
    envp.push_back(const_cast<char *>(it->c_str()));
  }
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
