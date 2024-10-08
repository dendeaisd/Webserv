#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP
#include <chrono>
#include <exception>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "HttpRequestEnums.hpp"

class HttpRequest {
 public:
  bool hasFile;
  HttpRequest();
  ~HttpRequest();

  HttpRequestHandler getHandler();
  std::string getMethod();
  HttpRequestMethod getMethodEnum();
  std::string getUri();
  std::string getQuery();
  std::string getHttpVersion();
  HttpRequestVersion getHttpVersionEnum();
  std::string getHost();
  std::string getSubDomain();
  std::string getPort();
  std::string getBody();
  std::string getHeader(std::string header);
  std::map<std::string, std::string> getHeaders();
  std::map<std::string, std::string> getQueryParams();
  std::map<std::string, std::string> getFormData();
  std::string getAttachment(std::string key);
  std::map<std::string, std::string> getAttachments();
  std::string getRequestTime();
  size_t getContentLength();
  bool checkTimeout();
  void setTimeoutSeconds(size_t seconds);
  void setHandler(HttpRequestHandler handler);
  bool setMethod(std::string method);
  void setMethod(HttpRequestMethod httpRequestMethod);
  void setUri(std::string uri);
  void setQuery(std::string query);
  void setHttpVersion(std::string httpVersion);
  void setHttpVersion(HttpRequestVersion httpProtocolVersion);
  void setHost(std::string host);
  void setSubDomain(std::string subDomain);
  void setPort(std::string port);
  void setBody(std::string body);
  void setHeader(std::string header, std::string value);
  void addFormData(std::string key, std::string value);
  void setHeaders(std::map<std::string, std::string> headers);
  void setQueryParam(std::string key, std::string value);
  void addAttachment(std::string key, std::string value);
  void addInjection(std::string injection);
  std::string getInjections();
  std::string toString();
  std::string toJson();

 private:
  std::string raw;
  std::string _request;
  std::string _method;
  HttpRequestMethod _httpRequestMethod;
  HttpRequestVersion _httpProtocolVersion;
  std::string _uri;
  std::string _query;
  std::string _httpVersion;
  std::string _host;
  std::string _port;
  std::string _body;
  std::string _subDomain;
  std::string _domain;
  HttpRequestHandler _handler;
  std::chrono::system_clock::time_point _requestTime;
  size_t _timeoutSeconds;
  std::map<std::string, std::string> _headers;
  std::map<std::string, std::string> _queryParams;
  std::map<std::string, std::string> _formData;
  std::map<std::string, std::string> _attachments;
  std::vector<std::string> _injections;
};

#endif
