#ifndef CONFIG_FILE
#define CONFIG_FILE

#include <memory>
#include <string>

#include "HttpContext.hpp"

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
