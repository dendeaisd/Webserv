#include "../../include/response/HttpResponse.hpp"

#include <fcntl.h>
#include <sys/socket.h>

#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>

#include "../../include/log/Log.hpp"
#include "../../include/request/Helpers.hpp"
#include "../../include/response/HttpReason.hpp"

HttpResponse::HttpResponse(int status) {
  _version = "HTTP/1.1";
  _statusCode = status;
  _reasonPhrase = HttpReason::getReasonPhrase(status);
  _body = "";
  _contentType = "text/html";
  _headers["Server"] = "Iridescent/0.1 (Unix)";
  // _headers["Connection"] = "close";
}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatusCode(int statusCode) {
  _statusCode = statusCode;
  _reasonPhrase = HttpReason::getReasonPhrase(statusCode);
}

void HttpResponse::setBody(std::string body) { _body = body; }

void HttpResponse::setHeader(std::string key, std::string value) {
  _headers[key] = value;
}

void HttpResponse::setContentType(std::string contentType) {
  _contentType = contentType;
}

void HttpResponse::setVersion(std::string version) { _version = version; }

void HttpResponse::setFile(std::string path) { _file = path; }

void HttpResponse::setFile(std::string path, std::string contentType) {
  _file = path;
  _contentType = contentType;
}

// This function allows for file viewing by setting the Content-Disposition to
// inline
void HttpResponse::setFile(std::string path, std::string contentType,
                           std::string disposition) {
  _file = path;
  _contentType = contentType;
  _headers["Content-Disposition"] = disposition;
}

// This function allows for file downloads by setting the Content-Disposition
void HttpResponse::setFile(std::string path, std::string contentType,
                           std::string disposition, std::string filename) {
  _file = path;
  _contentType = contentType;
  _headers["Content-Disposition"] = disposition + "; filename=" + filename;
}

std::string HttpResponse::loadFile() {
  std::ifstream file(_file, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(file)),
                      (std::istreambuf_iterator<char>()));
  file.close();
  return content;
}

std::string HttpResponse::getResponse() {
  if (_file != "") setBody(loadFile());
  std::string response = _version + " " + std::to_string(_statusCode) + " " +
                         _reasonPhrase + "\r\n";
  _headers["Content-Type"] = _contentType;
  _headers["Content-Length"] = std::to_string(_body.length());
  for (auto const &header : _headers) {
    response += header.first + ": " + header.second + "\r\n";
  }
  response += "\r\n";
  response += _body;
  return response;
}

bool HttpResponse::sendHeaders(int fd) {
  std::string response = _version + " " + std::to_string(_statusCode) + " " +
                         _reasonPhrase + "\r\n";
  _headers["Content-Type"] = _contentType;

  // Get file size
  std::ifstream file(_file, std::ios::binary | std::ios::ate);
  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  _headers["Content-Length"] = std::to_string(fileSize);
  if (_contentType.find("octet-stream") != std::string::npos)
    _headers["Content-Disposition"] =
        "attachment; filename=" + Helpers::getFilenameFromPath(_file);
  else
    _headers["Content-Disposition"] = "inline";
  _headers["Connection"] = "keep-alive";
  for (auto const &header : _headers) {
    response += header.first + ": " + header.second + "\r\n";
  }
  response += "\r\n";
  int sent = send(fd, response.c_str(), response.length(), 0);
  if (sent == -1)
  {
    Log::getInstance().error("Failed to send headers");
	return false;
  }
  if (sent == 0) return true;
  return true;
}

bool HttpResponse::sendResponse(int fd) {
  bool headerSent = sendHeaders(fd);
  if (!headerSent) return false;
  std::ifstream file(_file, std::ios::binary);
  if (!file.is_open()) {
    Log::getInstance().error("Failed to open file: " + _file);
    return false;
  }
  const std::size_t bufferSize = 8192;  // 8 KB buffer
  char buffer[bufferSize];
  while (file.read(buffer, bufferSize) || file.gcount() > 0) {
    size_t bytesToSend = file.gcount();
    size_t totalSent = 0;

    while (totalSent < bytesToSend) {
      ssize_t bytesSent =
          send(fd, buffer + totalSent, bytesToSend - totalSent, 0);
      if (bytesSent < 0) {
        // if (errno == EWOULDBLOCK || errno == EAGAIN) {
        //   // sleep a bit
        //   std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //   continue;
        // }
        return false;
      }
      totalSent += bytesSent;
    }
  }
  file.close();
  return true;
}

int HttpResponse::getStatusCode() { return _statusCode; }