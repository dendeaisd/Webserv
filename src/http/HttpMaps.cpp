#include "../../include/request/HttpMaps.hpp"

const std::map<std::string, HttpRequestMethod> HttpMaps::httpRequestMethodMap =
    initHttpRequestMethodMap();

const std::map<std::string, HttpRequestVersion>
    HttpMaps::httpRequestVersionMap = initHttpRequestVersionMap();

const std::set<std::string> HttpMaps::headerSet = initHeaderSet();
const std::set<std::string> HttpMaps::requiredHeaders1_1 =
    initRequiredHeaders1_1();

std::map<std::string, HttpRequestMethod> HttpMaps::initHttpRequestMethodMap() {
  std::map<std::string, HttpRequestMethod> map;
  map["GET"] = GET;
  map["POST"] = POST;
  map["PUT"] = PUT;
  map["DELETE"] = DELETE;
  map["HEAD"] = HEAD;
  map["OPTIONS"] = OPTIONS;
  map["PATCH"] = PATCH;
  map["TRACE"] = TRACE;
  map["CONNECT"] = CONNECT;
  return map;
}

std::map<std::string, HttpRequestVersion>
HttpMaps::initHttpRequestVersionMap() {
  std::map<std::string, HttpRequestVersion> map;
  map["HTTP/1.0"] = HTTP_1_0;
  map["HTTP/1.1"] = HTTP_1_1;
  map["HTTP/2.0"] = HTTP_2_0;
  return map;
}

std::set<std::string> HttpMaps::initHeaderSet() {
  std::set<std::string> set;
  set.insert("Host");
  set.insert("User-Agent");
  set.insert("Accept");
  set.insert("Accept-Language");
  set.insert("Accept-Encoding");
  set.insert("Connection");
  set.insert("Upgrade-Insecure-Requests");
  set.insert("Cache-Control");
  set.insert("Content-Type");
  set.insert("Content-Length");
  set.insert("Origin");
  set.insert("Referer");
  set.insert("Cookie");
  set.insert("Set-Cookie");
  set.insert("Authorization");
  return set;
}

std::set<std::string> HttpMaps::initRequiredHeaders1_1() {
  std::set<std::string> set;
  set.insert("Host");
  return set;
}
