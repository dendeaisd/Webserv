#ifndef CONFIG_FILE
#define CONFIG_FILE

#include "HttpContext.hpp"
#include <string>
#include <memory>

class ConfigFile {
 public:
  ConfigFile();
  void printConfigFile();
  std::string _workerProcessesValue;
  std::string _pidValue;
  std::string _errorLogValue;

  HttpContext _httpContext;
};

#endif
