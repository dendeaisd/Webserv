#ifndef HTTP_CONTEXT_HPP
#define HTTP_CONTEXT_HPP

#include <iostream>
#include <vector>
#include <memory>

#include "ServerContext.hpp"

class HttpContext {
 public:
  HttpContext();

  void addNewEmptyServer();

  std::vector<std::unique_ptr<ServerContext>> _serverContext;
  unsigned int _numberOfServers;
  Location _locationContext;

  std::string _geoipCountry;
  std::string _serverValue;
  std::string _server_nameValue;
  std::string _listenValue;
  std::string _rootValue;
  std::string _indexValue;
  std::string _ssl_certificateValue;
  std::string _ssl_certificate_keyValue;
  std::string _proxy_passValue;
  std::string _proxy_set_headerValue;
  std::string _upstreamValue;
  std::string _proxy_cache_pathValue;
  std::string _proxy_cacheValue;
  std::string _proxy_cache_validValue;
  std::string _proxy_cache_use_staleValue;
  std::string _rewriteValue;
  std::string _gzipValue;
  std::string _gzip_typesValue;
  std::string _allowValue;
  std::string _denyValue;
  std::string _access_logValue;
  std::string _error_logValue;
  std::string _limit_req_zoneValue;
  std::string _limit_reqValue;
};

#endif  // HTTP_CONTEXT_HPP
