#include "../../include/request/HttpRequest.hpp"

HttpRequest::HttpRequest()
    : _httpRequestMethod(HttpRequestMethod::UNKNOWN),
      _httpProtocolVersion(HttpRequestVersion::UNKNOWN) {}

HttpRequest::~HttpRequest() {
  _queryParams.clear();
  _headers.clear();
}

std::string HttpRequest::getMethod() { return _method; }
HttpRequestMethod HttpRequest::getMethodEnum() { return _httpRequestMethod; }

std::string HttpRequest::getUri() { return _uri; }

std::string HttpRequest::getHttpVersion() { return _httpVersion; }
HttpRequestVersion HttpRequest::getHttpVersionEnum() {
  return _httpProtocolVersion;
}

std::string HttpRequest::getHost() { return _host; }

std::string HttpRequest::getSubDomain() { return _subDomain; }

std::string HttpRequest::getPort() { return _port; }

std::string HttpRequest::getBody() { return _body; }

void HttpRequest::setQuery(std::string query) { _query = query; }

std::string HttpRequest::getHeader(std::string header) {
  if (_headers.find(header) != _headers.end()) {
    return _headers[header];
  } else {
    return "";
  }
}

std::map<std::string, std::string> HttpRequest::getHeaders() {
  return _headers;
}

std::map<std::string, std::string> HttpRequest::getQueryParams() {
  return _queryParams;
}

void HttpRequest::setMethod(std::string method) { _method = method; }
void HttpRequest::setMethod(HttpRequestMethod httpRequestMethod) {
  _httpRequestMethod = httpRequestMethod;
}

void HttpRequest::setUri(std::string uri) { _uri = uri; }

void HttpRequest::setHttpVersion(std::string httpVersion) {
  _httpVersion = httpVersion;
}

void HttpRequest::setHttpVersion(HttpRequestVersion httpProtocolVersion) {
  _httpProtocolVersion = httpProtocolVersion;
}

void HttpRequest::setHost(std::string host) { _host = host; }

void HttpRequest::setSubDomain(std::string subDomain) {
  _subDomain = subDomain;
}

void HttpRequest::setPort(std::string port) { _port = port; }

void HttpRequest::setBody(std::string body) { _body = body; }

void HttpRequest::setHeader(std::string header, std::string value) {
  _headers[header] = value;
}

void HttpRequest::setHeaders(std::map<std::string, std::string> headers) {
  _headers = headers;
}

void HttpRequest::setQueryParam(std::string key, std::string value) {
  _queryParams[key] = value;
}

std::string HttpRequest::toString() {
  std::string str = _method + " " + _uri;
  if (!_queryParams.empty()) {
    str += "?";
    for (std::map<std::string, std::string>::iterator it = _queryParams.begin();
         it != _queryParams.end(); it++) {
      str += it->first + "=" + it->second;
      if (it != --_queryParams.end()) {
        str += "&";
      }
    }
  }
  str += " " + _httpVersion + "\r\n";
  for (std::map<std::string, std::string>::iterator it = _headers.begin();
       it != _headers.end(); it++) {
    str += it->first + ": " + it->second + "\r\n";
  }
  str += "\r\n";
  if (!_body.empty()) {
    str += _body;
  }
  return str;
}

std::string HttpRequest::toJson() {
  std::string str = "{";
  str += "\"method\": \"" + _method + "\",";
  str += "\"uri\": \"" + _uri + "\",";
  str += "\"httpVersion\": \"" + _httpVersion + "\",";
  str += "\"host\": \"" + _host + "\",";
  str += "\"port\": \"" + _port + "\",";
  str += "\"subDomain\": \"" + _subDomain + "\",";
  str += "\"body\": \"" + _body + "\",";
  str += "\"headers\": {";
  for (std::map<std::string, std::string>::iterator it = _headers.begin();
       it != _headers.end(); it++) {
    str += "\"" + it->first + "\": \"" + it->second + "\"";
    if (it != --_headers.end()) {
      str += ",";
    }
  }
  str += "},";
  str += "\"queryParams\": {";
  for (std::map<std::string, std::string>::iterator it = _queryParams.begin();
       it != _queryParams.end(); it++) {
    str += "\"" + it->first + "\": \"" + it->second + "\"";
    if (it != --_queryParams.end()) {
      str += ",";
    }
  }
  str += "}";
  str += "}";
  return str;
}

HttpRequestHandler HttpRequest::getHandler() { return _handler; }

void HttpRequest::setHandler(HttpRequestHandler handler) { _handler = handler; }
