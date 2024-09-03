#include "../../include/request/Helpers.hpp"

#include <iostream>
#include <sstream>

bool Helpers::boundaryUpcoming(std::stringstream &ss,
                               const std::string &boundary) {
  int originalPos = ss.tellg();
  std::string line;
  std::getline(ss, line);
  if (line.find(boundary) != std::string::npos) {
    ss.clear();
    ss.seekg(originalPos);
    return true;
  }
  ss.clear();
  ss.seekg(originalPos);
  return false;
}

int Helpers::countRemainingLines(std::stringstream &ss) {
  int originalPos = ss.tellg();
  int count = 0;
  std::string line;
  while (std::getline(ss, line)) {
    count++;
  }
  ss.clear();
  ss.seekg(originalPos);
  return count;
}

std::string Helpers::getFormKeyIfExists(std::string data) {
  std::string key = "";
  std::string attr;
  if (data.find("form-data; ") != std::string::npos &&
      data.find("filename=") == std::string::npos) {
    attr = "name=";
    size_t pos = data.find(attr);
    key = data.substr(pos + attr.length());
    if (key.front() == '"' && key.back() == '"') {
      key = key.substr(1, key.length() - 2);
    }
  }
  return key;
}

std::string Helpers::getFilenameIfExists(std::string data) {
  std::string filename = "";
  std::string attr = "filename=";
  size_t pos = data.find(attr);
  if (pos != std::string::npos) {
    filename = data.substr(pos + attr.length());
    if (filename.front() == '"' && filename.back() == '"') {
      filename = filename.substr(1, filename.length() - 2);
    }
  }
  return filename;
}

std::string Helpers::getExtension(const std::string &path) {
  size_t pos = path.find_last_of('.');
  if (pos == std::string::npos) {
    return "";
  }
  return path.substr(pos + 1);
}

std::string Helpers::decodeUrl(const std::string &url) {
  std::string decoded;
  for (size_t i = 0; i < url.length(); i++) {
    if (url[i] == '%') {
      int value;
      sscanf(url.substr(i + 1, 2).c_str(), "%x", &value);
      decoded += static_cast<char>(value);
      i += 2;
    } else {
      decoded += url[i];
    }
  }
  return decoded;
}

std::string Helpers::getFilenameFromPath(const std::string &path) {
  size_t pos = path.find_last_of('/');
  if (pos == std::string::npos) {
    return path;
  }
  return path.substr(pos + 1);
}