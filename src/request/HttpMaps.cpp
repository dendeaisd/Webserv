#include "../../include/request/HttpMaps.hpp"
#include "../../include/log/Log.hpp"
#include <fstream>

HttpMaps::HttpMaps() {
  httpRequestMethodMap = initHttpRequestMethodMap();
  httpRequestVersionMap = initHttpRequestVersionMap();
  headerSet = initHeaderSet();
  requiredHeaders1_1 = initRequiredHeaders1_1();
}

HttpMaps::~HttpMaps() {
  httpRequestMethodMap.clear();
  httpRequestVersionMap.clear();
  headerSet.clear();
  requiredHeaders1_1.clear();
}

std::unordered_map<std::string, HttpRequestMethod>
HttpMaps::initHttpRequestMethodMap() {
  std::unordered_map<std::string, HttpRequestMethod> map;
  map["GET"] = HttpRequestMethod::GET;
  map["POST"] = HttpRequestMethod::POST;
  map["PUT"] = HttpRequestMethod::PUT;
  map["DELETE"] = HttpRequestMethod::DELETE;
  map["HEAD"] = HttpRequestMethod::HEAD;
  map["OPTIONS"] = HttpRequestMethod::OPTIONS;
  map["PATCH"] = HttpRequestMethod::PATCH;
  map["TRACE"] = HttpRequestMethod::TRACE;
  map["CONNECT"] = HttpRequestMethod::CONNECT;
  return map;
}

std::unordered_map<std::string, HttpRequestVersion>
HttpMaps::initHttpRequestVersionMap() {
  std::unordered_map<std::string, HttpRequestVersion> map;
  map["HTTP/1.0"] = HttpRequestVersion::HTTP_1_0;
  map["HTTP/1.1"] = HttpRequestVersion::HTTP_1_1;
  map["HTTP/2.0"] = HttpRequestVersion::HTTP_2_0;
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
  set.insert("Content-Disposition");
  set.insert("Expect");
  return set;
}

std::set<std::string> HttpMaps::initRequiredHeaders1_1() {
  std::set<std::string> set;
  set.insert("Host");
  return set;
}

std::string HttpMaps::getMimeType(const std::string &path) {
  size_t pos = path.find_last_of('.');
  if (pos == std::string::npos) {
    return "text/plain";
  }
  std::string extension = path.substr(pos + 1);
  std::unordered_map<std::string, std::string> mimeMap = {
      {"html", "text/html"},        {"htm", "text/html"},
      {"css", "text/css"},          {"js", "application/javascript"},
      {"json", "application/json"}, {"jpg", "image/jpeg"},
      {"jpeg", "image/jpeg"},       {"png", "image/png"},
      {"gif", "image/gif"},         {"ico", "image/x-icon"},
      {"pdf", "application/pdf"},   {"zip", "application/zip"},
      {"txt", "text/plain"}};

  auto it = mimeMap.find(extension);
  if (it != mimeMap.end()) {
    return it->second;
  }
  return "application/octet-stream";
}

HttpRequestMethod HttpMaps::getMethodEnum(std::string method) {
  return httpRequestMethodMap.find(method) != httpRequestMethodMap.end()
             ? httpRequestMethodMap.at(method)
             : HttpRequestMethod::UNKNOWN;
}

HttpRequestVersion HttpMaps::getVersionEnum(std::string version) {
  return httpRequestVersionMap.find(version) != httpRequestVersionMap.end()
             ? httpRequestVersionMap.at(version)
             : HttpRequestVersion::UNKNOWN;
}

bool HttpMaps::isHeaderValid(const std::string &header) {
  return headerSet.find(header) != headerSet.end();
}

bool HttpMaps::isHeaderRequired1_1(const std::string &header) {
  return requiredHeaders1_1.find(header) != requiredHeaders1_1.end();
}

bool HttpMaps::errorHasDefaultPage(int statusCode) {
  switch (statusCode) {
	case 401:
	case 403:
	case 404:
	case 500:
	case 504:
	  return true;
	default:
	  return false;
  }
}

std::string HttpMaps::getErrorPage(int statusCode) {
  std::string path = "./default/error_pages/";
  switch (statusCode) {
	case 401:
	  path += "401/401.html";
	  break;
	case 403:
	  path += "403/403.html";
	  break;
	case 404:
	  path += "404/404.html";
	  break;
	case 500:
	  path += "500/500.html";
	  break;
	case 504:
	  path += "504/504.html";
	  break;
	default:
	  path += "404/404.html";
	  break;
  }
  std::string content;
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    Log::getInstance().error("Failed to open status page file: " + path);
    return "";
  }
  return std::string((std::istreambuf_iterator<char>(file)),
					 (std::istreambuf_iterator<char>()));
}
