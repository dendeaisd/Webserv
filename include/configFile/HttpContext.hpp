#ifndef HTTP_CONTEXT_HPP
#define HTTP_CONTEXT_HPP

#include <iostream>
#include "ServerContext.hpp"

class HttpContext
{
  public:
    HttpContext();

    ServerContext serverContext_;

    std::string geoipCountry_;
    std::string serverValue_;
    std::string server_nameValue_;
    std::string listenValue_;
    std::string rootValue_;
    std::string indexValue_;
    std::string ssl_certificateValue_;
    std::string ssl_certificate_keyValue_;
    std::string proxy_passValue_;
    std::string proxy_set_headerValue_;
    std::string upstreamValue_;
    std::string proxy_cache_pathValue_;
    std::string proxy_cacheValue_;
    std::string proxy_cache_validValue_;
    std::string proxy_cache_use_staleValue_;
    std::string rewriteValue_;
    std::string gzipValue_;
    std::string gzip_typesValue_;
    std::string allowValue_;
    std::string denyValue_;
    std::string access_logValue_;
    std::string error_logValue_;
    std::string limit_req_zoneValue_;
    std::string limit_reqValue_;
    std::string geoip_countryValue_;
    std::string returnValue_;
    std::string fastcgi_passValue_;
    std::string fastcgi_indexValue_;
    std::string includeValue_;
    std::string uwsgi_passValue_;
    std::string scgi_passValue_;
    std::string mp4Value_;
    std::string mp4_buffer_sizeValue_;
    std::string mp4_max_buffer_sizeValue_;
    std::string sub_filterValue_;
    std::string sub_filter_onceValue_;
};

#endif // HTTP_CONTEXT_HPP
