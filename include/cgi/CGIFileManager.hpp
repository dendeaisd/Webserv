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
  CGIFileManager(std::string cgiDir);

  std::string getExecutor(std::string path);

 private:
  std::vector<CGIFile> cgiFiles_;
  std::map<std::string, std::string> cgiExecutors_;
  std::string cgiDir_;
  void mapCGIDir();
};

#endif