#include "HttpRequestParser.hpp"

HttpRequestParser::HttpRequestParser(const std::string request) {
 raw = request;
 status = HttpRequestParseStatus::NOT_PARSED;
}

HttpRequestParser::~HttpRequestParser() {
 headers.clear();
 queryParams.clear();
}

void HttpRequestParser::parse() {
 std::stringstream ss(raw);
 std::string requestLine;
 std::getline(ss, requestLine);
 parseRequestLine((char *)requestLine.c_str());
 if (status == HttpRequestParseStatus::INVALID || status == HttpRequestParseStatus::INCOMPLETE) {
  return;  // TODO: raise exception
 }
 std::string header;
 while (std::getline(ss, header) && header != "\r\n") {
  size_t pos = header.find(": ");
  if (pos != std::string::npos) {
   std::string key = header.substr(0, pos);
   if (headerSet.find(key) == headerSet.end()) {
    status = HttpRequestParseStatus::INVALID;
    return;
   }
   std::string value = header.substr(pos + 2);
   headers[key] = value;
  } else {
   status = HttpRequestParseStatus::INVALID;
   return;
  }
 }
 if (headers.find("Host") != headers.end()) {
  size_t pos = headers["Host"].find(":");
  if (pos != std::string::npos) {
   host = headers["Host"].substr(0, pos);
   port = headers["Host"].substr(pos + 1);
  } else {
   host = headers["Host"];
   port = "80";
  }
 } else {
  status = HttpRequestParseStatus::INVALID;
  return;
 }
 std::string query;
 size_t pos = uri.find("?");
 if (pos != std::string::npos) {
  query = uri.substr(pos + 1);
  uri = uri.substr(0, pos);
  parseQueryParams(query);
 }
 // TODO: check if body is present
 // TODO: check if file is present
 // TODO: check if request method support body
 std::string body;
 while (std::getline(ss, body)) {
  this->body += body;
 }
}

void HttpRequestParser::parseRequestLine(char *requestLine) {
 int i = 0;
 size_t len = std::strlen(requestLine);
 for (i = 0; i < len; i++) {
  if (requestLine[i] == ' ') {
   method = std::string(requestLine, i);
   break;
  }
 }
 if (!validateRequestMethod()) {
  status = HttpRequestParseStatus::INVALID;
  return;
 }
 int j = i + 1;
 for (i = j; i < len; i++) {
  if (requestLine[i] == ' ') {
   uri = std::string(requestLine + j, i - j);
   break;
  }
 }
 if (i == len || uri.empty() || uri[0] != '/') {
  status = HttpRequestParseStatus::INCOMPLETE;
  return;
 }
 j = i + 1;
 for (i = j; i < len; i++) {
  if (requestLine[i] == '\r' && requestLine[i + 1] == '\n') {
   httpVersion = std::string(requestLine + j, i - j);
   break;
  }
 }
 if (!validateHttpVersion()) {
  status = HttpRequestParseStatus::INVALID;
  return;
 }
}

bool HttpRequestParser::validateRequestMethod() {
 if (httpRequestMethodMap.find(method) != httpRequestMethodMap.end()) {
  httpRequestMethod = httpRequestMethodMap[method];
  return true;
 } else {
  httpRequestMethod = HttpRequestMethod::UNKNOWN;
  return false;
 }
}

bool HttpRequestParser::validateHttpVersion() {
 if (httpRequestVersionMap.find(httpVersion) != httpRequestVersionMap.end()) {
  httpProtocolVersion = httpRequestVersionMap[httpVersion];
  return true;
 } else {
  httpProtocolVersion = HttpRequestVersion::UNKNOWN;
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
   queryParams[key] = value;
  }
 }
}

std::string HttpRequestParser::getMethod() {
 return method;
}

std::string HttpRequestParser::getUri() {
 return uri;
}

std::string HttpRequestParser::getHttpVersion() {
 return httpVersion;
}

std::string HttpRequestParser::getHost() {
 return host;
}

std::string HttpRequestParser::getPort() {
 return port;
}

std::string HttpRequestParser::getBody() {
 return body;
}

std::string HttpRequestParser::getHeader(std::string header) {
 if (headers.find(header) != headers.end()) {
  return headers[header];
 } else {
  return "";
 }
}

std::map<std::string, std::string> HttpRequestParser::getHeaders() {
 return headers;
}

void HttpRequestParser::setMethod(std::string method) {
 method = method;
}

void HttpRequestParser::setUri(std::string uri) {
 uri = uri;
}

void HttpRequestParser::setHttpVersion(std::string httpVersion) {
 httpVersion = httpVersion;
}

void HttpRequestParser::setHost(std::string host) {
 host = host;
}

void HttpRequestParser::setPort(std::string port) {
 port = port;
}

void HttpRequestParser::setBody(std::string body) {
 body = body;
}

void HttpRequestParser::setHeader(std::string header, std::string value) {
 headers[header] = value;
}

void HttpRequestParser::setHeaders(std::map<std::string, std::string> headers) {
 headers = headers;
}

std::string HttpRequestParser::toString() {
 std::string str = method + " " + uri;
 if (!queryParams.empty()) {
  str += "?";
  for (std::map<std::string, std::string>::iterator it = queryParams.begin(); it != queryParams.end(); it++) {
   str += it->first + "=" + it->second;
   if (it != --queryParams.end()) {
    str += "&";
   }
  }
 }
 str += " " + httpVersion + "\r\n";
 for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++) {
  str += it->first + ": " + it->second + "\r\n";
 }
 str += "\r\n";
 if (!body.empty()) {
  str += body;
 }
}
