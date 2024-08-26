#include <map>
#include <set>
#include <string>
#include <unordered_map>

#include "HttpRequestEnums.hpp"

class HttpMaps {
 public:
  static const std::unordered_map<std::string, HttpRequestMethod>
      httpRequestMethodMap;
  static const std::unordered_map<std::string, HttpRequestVersion>
      httpRequestVersionMap;
  static const std::set<std::string> requiredHeaders1_1;
  static const std::set<std::string> headerSet;

 private:
  HttpMaps();
  ~HttpMaps();
  static std::unordered_map<std::string, HttpRequestMethod>
  initHttpRequestMethodMap();
  static std::unordered_map<std::string, HttpRequestVersion>
  initHttpRequestVersionMap();
  static std::set<std::string> initHeaderSet();
  static std::set<std::string> initRequiredHeaders1_1();
};