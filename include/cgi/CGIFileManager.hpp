#ifndef CGI_FILE_MANAGER_HPP
#define CGI_FILE_MANAGER_HPP
#include <map>
#include <string>
#include <vector>

struct CGIFile {
  std::string path;
  std::string executor;

  CGIFile(std::string path, std::string executor)
      : path(path), executor(executor) {}
};

class CGIFileManager {
 public:
  CGIFileManager(const CGIFileManager &) = delete;
  CGIFileManager &operator=(const CGIFileManager &) = delete;
  std::string getExecutor(std::string path);
  static CGIFileManager &getInstance() {
    static CGIFileManager _instance;
    return _instance;
  }
  void configure(std::string cgiDir);

 private:
  bool _configured;
  CGIFileManager() = default;
  std::string _cgiDir;
  std::vector<CGIFile> cgiFiles_;
  std::map<std::string, std::string> cgiExecutors_;
  std::string cgiDir_;
  void mapCGIDir();
};

#endif