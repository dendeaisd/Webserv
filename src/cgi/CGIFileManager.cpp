#include "../../include/cgi/CGIFileManager.hpp"

#include <filesystem>
#include <iostream>

#include "../../include/log/Log.hpp"

namespace fs = std::filesystem;

void CGIFileManager::configure(const ConfigFile& configFile,
                               std::string cgiDir) {
  if (_configured) {
    Log::getInstance().warning("CGIFileManager already configured");
    return;
  }
  collectExecutorMappings(configFile);
  cgiDir_ = cgiDir;
  mapCGIDir(cgiDir_);
  for (auto& file : cgiFiles_) {
    std::cout << file.path << " " << file.executor << std::endl;
  }
}

void CGIFileManager::collectExecutorMappings(const ConfigFile& configFile) {
  const HttpContext& httpContext = configFile._httpContext;

  for (const auto& serverContextPtr : httpContext._serverContext) {
    const ServerContext& serverContext = *serverContextPtr;
    for (const auto& locationPtr : serverContext._locationContext) {
      const Location& location = *locationPtr;
      for (const auto& cgiPair : location._cgi) {
        const std::string& extension = cgiPair.first;
        const std::string& executor = cgiPair.second;
        cgiExecutors_[extension] = executor;
      }
    }
  }
}

std::string CGIFileManager::getExecutor(std::string scriptPath) {
  size_t dotPos = scriptPath.find_last_of('.');
  if (dotPos == std::string::npos) {
    return "";
  }
  std::string extension = scriptPath.substr(dotPos);
  auto it = cgiExecutors_.find(extension);
  if (it != cgiExecutors_.end()) {
    return it->second;
  }
  return "";
}

void CGIFileManager::mapCGIDir(std::string cgiDir) {
  for (const auto& entry : fs::directory_iterator(cgiDir)) {
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
