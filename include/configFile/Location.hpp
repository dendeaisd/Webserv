#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <unordered_map>

class Location {
 public:
  Location();

  std::unordered_map<std::string, std::string> cgi;
  std::string _urlValue;
  std::string _rootValue;
  std::string _proxy_passValue;
  std::string _aliasValue;
  std::string _try_filesValue;
  std::string _indexValue;
  std::string _error_pageValue;
  std::string _access_logValue;
  std::string _denyValue;
};

#endif  // LOCATION_HPP
