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

CGI::CGI(int fd, HttpRequest &request) {
  _fd = fd;
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
  if (pipe(_pipeInFd) == -1 || pipe(_pipeOutFd) == -1) {
    Log::getInstance().error("Failed to create pipe");
    _unableToExecute = true;
  }
}

CGI::~CGI() {}

void CGI::sendTimeoutResponse() {
  _response = HttpResponse(408);
  _response.setBody(
      "<h2>Request timed out, we apologize for our garbage, unoptimized "
      "code.</h2>");
  Log::getInstance().error("Request timed out is sent!");
}

void CGI::sendInternalErrorResponse() { _response = HttpResponse(500); }

bool CGI::run() {
  if (_unableToExecute) {
    sendInternalErrorResponse();
    return false;
  }

  Log::getInstance().debug("Request: " + _request.toJson());
  _pid = fork();
  if (_pid == -1) {
    throw std::runtime_error("Failed to fork");
  }
  if (_pid == 0) {
    close(_pipeInFd[1]);
    close(_pipeOutFd[0]);
    dup2(_pipeInFd[0], STDIN_FILENO);
    dup2(_pipeOutFd[1], STDOUT_FILENO);
    close(_pipeInFd[0]);
    close(_pipeOutFd[1]);
    executeCGI();
    throw std::runtime_error("Failed to execute script");
  } else {
    close(_pipeInFd[0]);
    close(_pipeOutFd[1]);

    if (!_request.getBody().empty()) {
      Log::getInstance().debug("Writing to pipe " + _request.getBody());
      write(_pipeInFd[1], _request.getBody().c_str(),
            _request.getBody().length());
      close(_pipeInFd[1]);
    }
  }
  return true;
}

void CGI::killChild() {
  if (_pid != 0) {
    kill(_pid, SIGKILL);
  }
}

bool CGI::handleTimeout() {
  sendTimeoutResponse();
  close(_pipeOutFd[0]);
  killChild();
  return true;
}

bool CGI::handleError(std::string logMessage) {
  Log::getInstance().error(logMessage);
  killChild();
  close(_pipeOutFd[0]);
  sendInternalErrorResponse();
  return true;
}

bool CGI::tunnelData() {
  char buffer[BUFFER_SIZE + 1];
  int bytes_read;
  int flags = fcntl(_pipeOutFd[0], F_GETFL, 0);
  fcntl(_pipeOutFd[0], F_SETFL, flags | O_NONBLOCK);
  bytes_read = read(_pipeOutFd[0], buffer, BUFFER_SIZE);
  Log::getInstance().debug("Read " + std::to_string(bytes_read) + " bytes");
  if (bytes_read > 0) {
    buffer[bytes_read] = '\0';
    int sent = send(_fd, buffer, bytes_read, 0);
    Log::getInstance().debug("Sent " + std::to_string(sent) + " bytes");
    if (sent < 0) {
      Log::getInstance().error("Failed to send response");
      close(_fd);
    }
    if (bytes_read == BUFFER_SIZE) {
      return tunnelData();
    } else {
      close(_pipeOutFd[0]);
    }
    return true;
  } else {
    if (bytes_read < 0) {
      Log::getInstance().error("Failed to read from pipe with error: " +
                               std::string(std::strerror(errno)));
    }
    close(_pipeOutFd[0]);
    sendInternalErrorResponse();
    send(_fd, _response.getResponse().c_str(), _response.getResponse().length(),
         0);
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
  pid_t result = waitpid(_pid, &status, WNOHANG);
  if (result == -1 || result == 0) {
    return false;
  }
  if (result == _pid) {
    if (WIFEXITED(status)) {
      if (WEXITSTATUS(status) != 0) {
        return handleError("Script exited with non-zero status");
      }
    } else {
      return handleError("Script did not exit normally");
    }
  }
  return true;
}

void CGI::executeCGI() {
  std::vector<char *> args;
  std::vector<char *> envp;
  std::vector<std::string> env_strs;

  args.push_back(const_cast<char *>(_language.c_str()));
  args.push_back(const_cast<char *>(_script.c_str()));
  args.push_back(nullptr);

  env_strs.push_back("CONTENT_LENGTH=" + _request.getHeader("Content-Length"));
  env_strs.push_back("CONTENT_TYPE=" + _request.getHeader("Content-Type"));
  env_strs.push_back("GATEWAY_INTERFACE=CGI/1.1");
  env_strs.push_back("REMOTE_ADDR=" + _request.getHost());
  env_strs.push_back("REQUEST_METHOD=" + _request.getMethod());
  env_strs.push_back("HOST=" + _request.getHost());
  env_strs.push_back("ORIGIN=" + _request.getHeader("Origin"));
  env_strs.push_back("URL_ARG=" + _urlArg);
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

bool CGI::handleResponse() {
  if (_response.getStatusCode() == -1) return tunnelData();
  send(_fd, _response.getResponse().c_str(), _response.getResponse().length(),
       0);
  return true;
}
