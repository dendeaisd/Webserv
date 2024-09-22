#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <unordered_map>
#include <vector>

class Location {
 public:
  Location();

  void printLocation();

  std::string _urlValue;

  //  std::string _rootValue;
  std::string _proxyPassValue;
  std::string _aliasValue;
  std::string _autoIndexValue;

  bool _returnSet;
  std::pair<int, std::string> _returnValues;

  //  std::vector<std::string> _indexValue;
  std::vector<std::string> _denyValue;
  std::vector<std::string> _includeValue;
  std::vector<std::string> _rewriteValue;
  std::vector<std::string> _tryFilesValue;
  std::vector<std::string> _errorPageValue;
  std::vector<std::string> _accessLogValue;
  std::unordered_map<std::string, std::string> _cgi;

 private:
  // void cgiSetSeparatedValue(const std::string &value);
  // void addReturn(const std::string &value);
  void printTypeFormat(const std::string &type,
                       const std::string &value) const noexcept;
  void printVecOfStrings(const std::vector<std::string> &vec,
                         const std::string &type) const noexcept;
};

#endif  // LOCATION_HPP
