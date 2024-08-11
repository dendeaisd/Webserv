#ifndef HTTP_CONTEXT_HPP
#define HTTP_CONTEXT_HPP

#include <iostream>
#include <vector>

#include "ServerContext.hpp"

class HttpContext {
 public:
  HttpContext();

  std::vector<ServerContext> _serverContext;
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
  std::string _geoip_countryValue;
  std::string _returnValue;
  std::string _fastcgi_passValue;
  std::string _fastcgi_indexValue;
  std::string _includeValue;
  std::string _uwsgi_passValue;
  std::string _scgi_passValue;
  std::string _mp4Value;
  std::string _mp4_buffer_sizeValue;
  std::string _mp4_max_buffer_sizeValue;
  std::string _sub_filterValue;
  std::string _sub_filter_onceValue;
};

#endif  // HTTP_CONTEXT_HPP
