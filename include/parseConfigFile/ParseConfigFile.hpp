#ifndef PARSE_CONFIG_FILE_HPP
#define PARSE_CONFIG_FILE_HPP

#include <string>
#include <fstream>

class ParseConfigFile
{
  public:
    std::fstream getValidFilePath(const std::string &file) const;

};

#endif
