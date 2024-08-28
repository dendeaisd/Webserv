#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include <iostream>
#include <stack>

#include "HttpContext.hpp"

enum class StoringStates {
  MAIN_CONTEXT = 0,
  HTTPS_CONTEXT = 1,
  SERVER_CONTEXT_IN_HTTP = 2,
  LOCATION_CONTEXT_IN_SERVER = 3
};

enum EBracketStatus {
  MAIN_BRACKET = 0,
  HTTP_BRACKET = 1,
  SERVER_BRACKET = 2,
  LOCATION_BRACKET = 3
};


class ConfigFile {
 public:
  ConfigFile();

  void printConfigFileContent();
  void parseConfiguration(const std::string &fileName);

  std::string _workerProcessesValue;
  std::string _pidValue;
  std::string _errorLogValue;

  HttpContext _httpContext;

 private:
  void setCurrentState(const std::string &line);
  void possibleNewServerContextSetup(const std::string &line);
  void trackBrackets(const std::string &line);
  EBracketStatus transferStateToBracketStatus();
  void mainContextSaveDirective(const std::string &line);
  void saveDirective(const std::string &line, std::string &directive);
  void getValue(const std::string &line, std::string &value);
  void httpContextSave(const std::string &line);
  void getKey(const std::string &line, std::string &key);
  void serverContextSave(const std::string &line);
  void locationContextSave(const std::string &line);
  void removeWhiteSpacesFront(std::string &str);
  int numberOfWordsSeparatedBySpaces(const std::string &str);
  bool isStoringState(const std::string &line, StoringStates state);
  void getStateAsString(std::string &stateStr, StoringStates &state);
  void possibleNewLocationSetup(std::string &line);

  StoringStates _state;
  std::stack<char> _bracketStatus[4];
};

#endif
