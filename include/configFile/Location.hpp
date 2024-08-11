#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>

class Location {
 public:
  Location();

  std::string _urlValue;
  std::string _rootValue;
  std::string _proxy_passValue;
  std::string _aliasValue;
  std::string _try_filesValue;
  std::string _indexValue;
  std::string _error_pageValue;
  std::string _rewriteValue;
  std::string _returnValue;
  std::string _access_logValue;
  std::string _denyValue;
  std::string _allowValue;
  std::string _add_headerValue;
  std::string _expiresValue;
  std::string _limit_reqValue;
  std::string _limit_connValue;
  std::string _proxy_set_headerValue;
  std::string _proxy_redirectValue;
  std::string _fastcgi_passValue;
  std::string _uwsgi_passValue;
  std::string _scgi_passValue;
  std::string _ContextsValue;
  std::string _ifValue;
  std::string _limit_exceptValue;
};

#endif  // LOCATION_HPP
