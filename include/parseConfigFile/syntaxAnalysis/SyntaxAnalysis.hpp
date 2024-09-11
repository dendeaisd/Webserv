#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <memory>

#include "ConfigFile.hpp"
#include "NodeToken.hpp"

enum class StoringStates {
  MAIN_CONTEXT = 0,
  HTTPS_CONTEXT = 1,
  SERVER_CONTEXT = 2,
  LOCATION_CONTEXT = 3
};

enum EBracketStatus {
  MAIN_BRACKET = 0,
  HTTP_BRACKET = 1,
  SERVER_BRACKET = 2,
  LOCATION_BRACKET = 3
};

class SyntaxAnalysis {
 public:

  void printConfigFileContent();
  void parseConfiguration(const std::string &fileName);


 private:
  StoringStates _state;
  std::stack<char> _bracketStatus[4];
};

#endif
