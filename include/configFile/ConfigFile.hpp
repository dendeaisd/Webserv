#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include <iostream>

#include "HttpContext.hpp"

class ConfigFile {
 public:
  ConfigFile();

  void storeConfiguration(const std::string &fileName);

  std::string _workerProcessesValue;
  std::string _pidValue;
  std::string _errorLogValue;

  HttpContext _httpContext;
};

#endif  // CONFIG_FILE_HPP
