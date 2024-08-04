#include "../../include/request/HttpRequestParser.hpp"  //also wouldn t recognize the path

// Also, instead of returning early and setting a status
// i would maybe throw different exceptions for different parsing errors
HttpRequestParser::HttpRequestParser(const std::string request)
    : status(NOT_PARSED), hasFile(false), raw(request) {}

HttpRequestParser::~HttpRequestParser() {}

HttpRequest HttpRequestParser::getHttpRequest() {
  if (status == PARSED) {
    return request;
  } else {
    return HttpRequest();
  }
}

int HttpRequestParser::parse() {
  std::stringstream ss(raw);
  std::string requestLine;
  std::getline(ss, requestLine);
  std::cout << requestLine << std::endl;
  if (requestLine.find("\r") != std::string::npos)
    requestLine.erase(requestLine.find("\r"), 1);
  parseRequestLine((char *)requestLine.c_str());
  if (status == INVALID || status == INCOMPLETE) {
    return 400;
  }
  parseHeaders(ss);
  if (status == INVALID) {
    return 400;
  }
  std::string query;
  size_t pos = request.getUri().find("?");
  if (pos != std::string::npos) {
    request.setQuery(request.getUri().substr(pos + 1));
    request.setUri(request.getUri().substr(0, pos));
    parseQueryParams(query);
  }
  if (request.getHeader("Content-Length") != "") {
    parseBody(ss);
  }
  return 200;
}

void HttpRequestParser::parseRequestLine(char *requestLine) {
  size_t i = 0;
  size_t len = std::strlen(requestLine);
  for (i = 0; i < len; i++) {
    if (requestLine[i] == ' ') {
      request.setMethod(std::string(requestLine, i));
      break;
    }
  }
  if (!validateRequestMethod()) {
    status = INVALID;
    return;
  }
  size_t j = i + 1;
  for (i = j; i < len; i++) {
    if (requestLine[i] == ' ') {
      request.setUri(std::string(requestLine + j, i - j));
      break;
    }
  }
  if (i == len || request.getUri().empty() || request.getUri()[0] != '/') {
    status = INCOMPLETE;
    return;
  }
  j = i + 1;
  request.setHttpVersion(std::string(requestLine + j, len - j));
  if (!validateHttpVersion()) {
    std::cout << "Invalid HTTP version" << std::endl;
    status = INVALID;
    return;
  }
}

void HttpRequestParser::parseHeaders(std::stringstream &ss) {
  std::string header;
  while (std::getline(ss, header) && (header != "\r" && header != "")) {
    /*
    No whitespace is allowed between the field name and colon.  In the
    past, differences in the handling of such whitespace have led to
    security vulnerabilities in request routing and response handling.
    -- src: https://www.rfc-editor.org/rfc/inline-errata/rfc9112.html
    */
    if (header.find(" : ") != std::string::npos) {
      status = INVALID;
      return;
    }
    size_t pos = header.find(": ");
    if (pos != std::string::npos) {
      std::string key = header.substr(0, pos);
      if (HttpMaps::headerSet.find(key) == HttpMaps::headerSet.end()) {
        status = INVALID;
        return;
      }
      if (header.find("\r") != std::string::npos)
        header.erase(header.find("\r"), 1);
      std::string value = header.substr(pos + 2);
      std::cout << "value " << value << std::endl;
      request.setHeader(key, value);
    } else {
      status = INVALID;
      return;
    }
  }
  std::string host = request.getHeader("Host");
  if (host != "") {
    size_t pos = host.find(":");
    if (pos != std::string::npos) {
      request.setHost(host.substr(0, pos));
      request.setPort(host.substr(pos + 1));
    } else {
      request.setHost(host);
      request.setPort("80");
    }
    status = PARSED;
  } else {
    status = INVALID;
    return;
  }
}

void HttpRequestParser::parseBody(std::stringstream &ss) {
  // TODO: check if body is present
  // TODO: check if file is present
  // TODO: check if request method support body
  std::string body;
  std::string line;
  while (std::getline(ss, line)) {
    body += line;
  }
  request.setBody(body);
}

bool HttpRequestParser::validateRequestMethod() {
  if (HttpMaps::httpRequestMethodMap.find(request.getMethod()) !=
      HttpMaps::httpRequestMethodMap.end()) {
    request.setMethod(HttpMaps::httpRequestMethodMap.at(
        HttpMaps::httpRequestMethodMap.find(request.getMethod())->first));
    return true;
  } else {
    request.setMethod(METHOD_UNKNOWN);
    return false;
  }
}

bool HttpRequestParser::validateHttpVersion() {
  if (HttpMaps::httpRequestVersionMap.find(request.getHttpVersion()) !=
      HttpMaps::httpRequestVersionMap.end()) {
    request.setHttpVersion(HttpMaps::httpRequestVersionMap.at(
        HttpMaps::httpRequestVersionMap.find(request.getHttpVersion())->first));
    return true;
  } else {
    request.setHttpVersion(VERSION_UNKNOWN);
    return false;
  }
}

void HttpRequestParser::parseQueryParams(std::string query) {
  std::stringstream ss(query);
  std::string param;
  while (std::getline(ss, param, '&')) {
    size_t pos = param.find("=");
    if (pos != std::string::npos) {
      std::string key = param.substr(0, pos);
      std::string value = param.substr(pos + 1);
      request.setQueryParam(key, value);
    }
  }
}
