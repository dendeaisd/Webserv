#ifndef PARSE_CONFIG_FILE_HPP
#define PARSE_CONFIG_FILE_HPP

#include <fstream>
#include <string>

#include "ConfigFile.hpp"

class ParseConfigFile {
 public:
  ParseConfigFile(const std::string &filePath);
  std::unique_ptr<ConfigFile> getConfigFile();
  ~ParseConfigFile() {};

 private:
  ParseConfigFile() = delete;
  ParseConfigFile(const ParseConfigFile &) = delete;
  ParseConfigFile &operator=(const ParseConfigFile &) = delete;

  std::ifstream getValidFilePath(const std::string &filePath) const;

  std::unique_ptr<ConfigFile> _config;
};

#endif
