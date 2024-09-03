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

#include "../../include/cgi/CGIFileManager.hpp"
#include "../../include/log/Log.hpp"
#include "../../include/response/HttpResponse.hpp"

#define BUFFER_SIZE 4096
#define TIMEOUT 20

CGI::CGI(int fd, HttpRequest &request) {
  fd_ = fd;
  _request = request;
  _unableToExecute = false;
  std::string uri = request.getUri();
  size_t dotPos = uri.find(".");
  if (dotPos == std::string::npos) {
    _unableToExecute = true;
    return;
  }
  _urlArg = "";
  size_t slashPos = uri.find("/", dotPos);
  if (slashPos != std::string::npos) {
    _urlArg = uri.substr(slashPos + 1);
    _script = "." + uri.substr(0, slashPos);
  } else {
    _script = "." + uri;
  }
  _language = CGIFileManager::getInstance().getExecutor(_script);
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

bool CGI::run() {
  if (_unableToExecute) {
    sendInternalErrorResponse(fd_);
    return false;
  }

  Log::getInstance().debug("Request: " + _request.toJson());
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

    if (!_request.getBody().empty()) {
      Log::getInstance().debug("Writing to pipe " + _request.getBody());
      write(pipeInFd_[1], _request.getBody().c_str(),
            _request.getBody().length());
      close(pipeInFd_[1]);
    }
  }
  return true;
}

void CGI::killChild() {
  if (pid_ != 0) {
    kill(pid_, SIGKILL);
  }
}

bool CGI::handleTimeout() {
  Log::getInstance().error("Request timed out");
  killChild();
  close(pipeOutFd_[0]);
  sendTimeoutResponse(fd_);
  return true;
}

bool CGI::handleError(std::string logMessage) {
  Log::getInstance().error(logMessage);
  killChild();
  close(pipeOutFd_[0]);
  sendInternalErrorResponse(fd_);
  return true;
}

bool CGI::tunnelData() {
  char buffer[BUFFER_SIZE + 1];
  int bytes_read;
  int flags = fcntl(pipeOutFd_[0], F_GETFL, 0);
  fcntl(pipeOutFd_[0], F_SETFL, flags | O_NONBLOCK);
  bytes_read = read(pipeOutFd_[0], buffer, BUFFER_SIZE);
  Log::getInstance().debug("Read " + std::to_string(bytes_read) + " bytes");
  if (bytes_read > 0) {
    buffer[bytes_read] = '\0';
    int sent = send(fd_, buffer, bytes_read, 0);
    Log::getInstance().debug("Sent " + std::to_string(sent) + " bytes");
    if (sent < 0) {
      Log::getInstance().error("Failed to send response");
      close(fd_);
    }
    if (bytes_read == BUFFER_SIZE) {
      return tunnelData();
    } else {
      close(pipeOutFd_[0]);
    }
    // if (_request.getHeader("Connection") != "keep-alive") {
    //   close(fd_);
    // }
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
  if (_request.checkTimeout()) {
    return handleTimeout();
  }
  pid_t result = waitpid(pid_, &status, WNOHANG);
  if (result == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // still needs time to respond
      return false;
    }
    return handleError("Failed to wait for child process");
  }
  if (result == 0) {
    return false;
  }
  if (result == pid_) {
    if (WIFEXITED(status)) {
      if (WEXITSTATUS(status) != 0) {
        return handleError("Script exited with non-zero status");
      }
    } else {
      return handleError("Script did not exit normally");
    }
  }
  return tunnelData();
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
  envp.push_back(const_cast<char *>(
      std::string("ORIGIN=" + _request.getHeader("Origin")).c_str()));
  envp.push_back(const_cast<char *>(std::string("URL_ARG=" + _urlArg).c_str()));
  if (_request.getHeader("Cookie") != "") {
    Log::getInstance().debug("Cookie: " + _request.getHeader("Cookie"));
    env_strs.push_back("HTTP_COOKIE=" + _request.getHeader("Cookie"));
  }
  auto qp = _request.getQueryParams();
  for (auto it = qp.begin(); it != qp.end(); ++it) {
    env_strs.push_back("QS_" + it->first + "=" + it->second);
  }
  auto form = _request.getFormData();
  for (auto it = form.begin(); it != form.end(); ++it) {
    env_strs.push_back("FORM_" + it->first + "=" + it->second);
  }

  for (auto it = env_strs.begin(); it != env_strs.end(); ++it) {
    envp.push_back(const_cast<char *>(it->c_str()));
  }
  envp.push_back(nullptr);

  execve(_language.c_str(), args.data(), envp.data());
  Log::getInstance().error("Failed to execute script");
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
  return 0;
}
