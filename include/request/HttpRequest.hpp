#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP
#include <exception>
#include <map>
#include <set>
#include <string>
// you are not using the sstream header direcly anywhere here
#include <sstream>

// #include "../FileUpload.hpp"  // path was not correct, i changed it
#include "HttpRequestEnums.hpp"

class HttpRequest {
 public:
  bool hasFile;
  HttpRequest();
  ~HttpRequest();
 
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
  void setMethod(std::string method);
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
  void setHeaders(std::map<std::string, std::string> headers);
  void setQueryParam(std::string key, std::string value);
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
  // FileUpload file;
  std::map<std::string, std::string> _headers;
  std::map<std::string, std::string> _queryParams;
  
};

#endif
