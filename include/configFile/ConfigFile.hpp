#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include <iostream>
#include <stack>
#include "states.hpp"

#include "HttpContext.hpp"

class ConfigFile {
 public:
  ConfigFile();

  void storeConfiguration(const std::string &fileName);

  std::string _workerProcessesValue;
  std::string _pidValue;
  std::string _errorLogValue;

  HttpContext _httpContext;

 private:
  void setCurrentState(const std::string &line);
  void trackBrackets(const std::string &line);
  void popBracketForCurrentState();
  void pushBracketForCurrentState();

  TStoringStates _state;
  std::stack<char> _httpBracket;
  std::stack<char> _serverBracket;
  std::stack<char> _locationBracket;
};

#endif  // CONFIG_FILE_HPP
