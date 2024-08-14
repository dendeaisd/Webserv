#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include <iostream>
#include <stack>

#include "HttpContext.hpp"
#include "states.hpp"

enum EBracketStatus {
  HTTP_BRACKET = 0,
  SERVER_BRACKET = 1,
  LOCATION_BRACKET = 2
};

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
  void possibleNewServerContextSetup(const std::string &line);
  void trackBrackets(const std::string &line);
  void transferStateToBracketStatus(EBracketStatus &status);

  TStoringStates _state;
  std::stack<char> _bracketStatus[3];
};

#endif  // CONFIG_FILE_HPP
