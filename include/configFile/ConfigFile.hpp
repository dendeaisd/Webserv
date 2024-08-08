#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include <iostream>
#include "HttpContext.hpp"

class ConfigFile {
 public:

  ConfigFile();

  std::string workerProcessesValue_;
  std::string pidValue_;
  std::string errorLogValue_;

  HttpContext httpContext_;
};

#endif  // CONFIG_FILE_HPP
