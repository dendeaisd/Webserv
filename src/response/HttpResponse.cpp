#include "../../include/response/HttpResponse.hpp"

#include <fstream>
#include <iostream>

#include "../../include/response/HttpReason.hpp"

HttpResponse::HttpResponse(int status) {
  _version = "HTTP/1.1";
  _statusCode = status;
  _reasonPhrase = HttpReason::getReasonPhrase(status);
  _body = "";
  _contentType = "text/html";
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
  setBody(loadFile());
}

// This function allows for file viewing by setting the Content-Disposition to
// inline
void HttpResponse::setFile(std::string path, std::string contentType,
                           std::string disposition) {
  _file = path;
  _contentType = contentType;
  _headers["Content-Disposition"] = disposition;
  setBody(loadFile());
}

// This function allows for file downloads by setting the Content-Disposition
void HttpResponse::setFile(std::string path, std::string contentType,
                           std::string disposition, std::string filename) {
  _file = path;
  _contentType = contentType;
  _headers["Content-Disposition"] = disposition + "; filename=" + filename;
  setBody(loadFile());
}

std::string HttpResponse::loadFile() {
  std::ifstream file(_file, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(file)),
                      (std::istreambuf_iterator<char>()));
  file.close();
  return content;
}

std::string HttpResponse::getResponse() {
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