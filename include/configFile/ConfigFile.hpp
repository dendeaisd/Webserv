#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include <iostream>
#include <stack>

#include "HttpContext.hpp"
#include "states.hpp"

enum EBracketStatus {
  MAIN_BRACKET = 0,
  HTTP_BRACKET = 1,
  SERVER_BRACKET = 2,
  LOCATION_BRACKET = 3
};

class ConfigFile {
 public:
  ConfigFile();

  void storeValidConfiguration(const std::string &fileName);

  std::string _workerProcessesValue;
  std::string _pidValue;
  std::string _errorLogValue;

  HttpContext _httpContext;

 private:
  void setCurrentState(const std::string &line);
  void possibleNewServerContextSetup(const std::string &line);
  void trackBrackets(const std::string &line);
  void transferStateToBracketStatus(EBracketStatus &status);
  void mainContextSaveDirective(const std::string &line);
  void saveDirective(const std::string &line, std::string &directive);
  void getValue(const std::string &line, std::string &value);
  void httpContextSave(const std::string &line);
  void getKey(const std::string &line, std::string &key);
  void serverContextSave(const std::string &line);
  void locationContextSave(const std::string &line);
  void removeWhiteSpacesFront(std::string &str);


  TStoringStates _state;
  std::stack<char> _bracketStatus[4];
};

#endif
