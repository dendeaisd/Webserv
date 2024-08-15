#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <unordered_map>
#include <vector>

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
  std::string _errorPageValue;
  std::string _accessLogValue;
  std::string _denyValue;
  std::vector<std::string> _rewriteValue;

};

#endif  // LOCATION_HPP
