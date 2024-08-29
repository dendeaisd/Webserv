#include "../../include/cgi/CGIFileManager.hpp"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void CGIFileManager::configure(std::string cgiDir) {
  // TEMP map for cgi executors
  if (_configured) {
    std::cout << "CGIFileManager is already configured" << std::endl;
    return;
  }
  cgiExecutors_[".php"] = "/usr/bin/php";
  cgiExecutors_[".py"] = "/usr/bin/python3";
  cgiExecutors_[".pl"] = "/usr/bin/perl";
  cgiExecutors_[".rb"] = "/usr/bin/ruby";
  cgiExecutors_[".sh"] = "/bin/bash";
  cgiDir_ = cgiDir;
  mapCGIDir(cgiDir_);
  for (auto &file : cgiFiles_) {
    std::cout << file.path << " " << file.executor << std::endl;
  }
}

std::string CGIFileManager::getExecutor(std::string path) {
  for (auto &file : cgiFiles_) {
    if (file.path == path) {
      return file.executor;
    }
  }
  return "";
}

void CGIFileManager::mapCGIDir(std::string cgiDir) {
  for (const auto &entry : fs::directory_iterator(cgiDir)) {
    if (entry.is_regular_file()) {
      std::string path = entry.path().string();
      std::string extension = path.substr(path.find_last_of('.'));
      if (cgiExecutors_.find(extension) != cgiExecutors_.end()) {
        cgiFiles_.push_back(CGIFile(path, cgiExecutors_[extension]));
      }
    } else if (entry.is_directory()) {
      mapCGIDir(entry.path().string());
    }
  }
}