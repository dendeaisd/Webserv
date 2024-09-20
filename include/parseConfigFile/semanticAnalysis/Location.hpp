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
  void printLocation();

  std::string _urlValue;

//  std::string _rootValue;
//  std::string _indexValue;
  std::string _proxyPassValue;
  std::string _aliasValue;
  std::string _autoIndexValue;

  bool returnSet;
  int _returnStatusCodeValue;
  std::pair<int, std::string> _statusCodeAndUrlReturnValue;

  std::vector<std::string> _denyValue;
  std::vector<std::string> _returnValue;
  std::vector<std::string> _includeValue;
  std::vector<std::string> _rewriteValue;
  std::vector<std::string> _tryFilesValue;
  std::vector<std::string> _errorPageValue;
  std::vector<std::string> _accessLogValue;
  std::unordered_map<std::string, std::string> _cgi;

 private:
  void cgiSetSeparatedValue(const std::string &value);
  void addReturn(const std::string &value);
};

#endif  // LOCATION_HPP
