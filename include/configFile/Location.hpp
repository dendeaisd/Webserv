#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <unordered_map>
#include <vector>

class Location {
 public:
  Location();

  void initializeLocation(const std::string &url);
  void locationSaveDirectiveValue(const std::string &key,
                                  const std::string &value);

  std::string _urlValue;

  std::string _rootValue;
  std::string _includeValue;
  std::string _proxyPassValue;
  std::string _aliasValue;
  std::string _tryFilesValue;
  std::string _indexValue;
  std::string _errorPageValue;
  std::string _accessLogValue;
  std::string _denyValue;
  std::unordered_map<std::string, std::string> _cgi;
  std::vector<std::string> _rewriteValue;

 private:
  void cgiSetSeparatedValue(const std::string &value);
};

#endif  // LOCATION_HPP
