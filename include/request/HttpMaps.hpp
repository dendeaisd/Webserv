#include <map>
#include <set>
#include <string>
#include <unordered_map>

#include "HttpRequestEnums.hpp"

class HttpMaps {
 public:
  static HttpMaps &getInstance() {
    static HttpMaps instance;
    return instance;
  }
  HttpMaps(const HttpMaps &) = delete;
  HttpMaps &operator=(const HttpMaps &) = delete;
  std::string getMimeType(const std::string &uri);
  HttpRequestMethod getMethodEnum(std::string method);
  HttpRequestVersion getVersionEnum(std::string version);
  bool isHeaderValid(const std::string &header);
  bool isHeaderRequired1_1(const std::string &header);

 private:
  HttpMaps();
  ~HttpMaps();
  std::unordered_map<std::string, HttpRequestMethod> initHttpRequestMethodMap();
  std::unordered_map<std::string, HttpRequestVersion>
  initHttpRequestVersionMap();
  std::set<std::string> initHeaderSet();
  std::set<std::string> initRequiredHeaders1_1();
  std::unordered_map<std::string, HttpRequestMethod> httpRequestMethodMap;
  std::unordered_map<std::string, HttpRequestVersion> httpRequestVersionMap;
  std::set<std::string> requiredHeaders1_1;
  std::set<std::string> headerSet;
};