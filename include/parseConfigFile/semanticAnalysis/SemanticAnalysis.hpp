#ifndef SEMANTIC_ANALYSIS_HPP
#define SEMANTIC_ANALYSIS_HPP

#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <set>

#include "ConfigFile.hpp"
#include "NodeToken.hpp"

typedef std::vector<std::vector<std::unique_ptr<TokenNode>>> TokenStructure;

enum class State {
  MAIN_CONTEXT = 0,
  HTTPS_CONTEXT = 1,
  SERVER_CONTEXT = 2,
  LOCATION_CONTEXT = 3
};

enum EBracketStatus {
  MAIN_BRACKET = 0,
  HTTP_BRACKET = 1,
  SERVER_BRACKET = 2,
  LOCATION_BRACKET = 3,
  ERROR = 4
};

class SemanticAnalysis {
 public:
  SemanticAnalysis() = delete;
  SemanticAnalysis(const SemanticAnalysis &) = delete;
  SemanticAnalysis &operator=(const SemanticAnalysis &other) = delete;
  SemanticAnalysis(TokenStructure &token);
  ~SemanticAnalysis();

  void setCurrentState();
  bool OneTokenInLineIsADirective() noexcept;
  bool canEnterHttpContext() noexcept;
  bool canEnterServerContext() noexcept;
  bool canEnterLocationContext() noexcept;
  bool backSwitchState(State state, EBracketStatus bracket) noexcept;

  void trackBrackets();
  bool bracketInLineOfTokens() noexcept;
  EBracketStatus getCurrentBracketStatus() noexcept;
  bool canTransitionToNewContext() noexcept;
  bool openBracketStateIs(TypeToken expectedType, State currentState) noexcept;
  bool moveStateBackFrom(State currentState) noexcept;
  bool validClosingBracket() noexcept;

  void possibleCreationOfNewContext() noexcept;
  void createServerContext() noexcept;
  void createLocationContext() noexcept;
  bool validServerLine() noexcept;
  bool validLocationLine() noexcept;
 private:
  ConfigFile _config;
  State _state;
  std::stack<char> _bracketStatus[4];
  std::vector<std::unique_ptr<TokenNode>> _tokenLine;
};

std::ostream &operator<<(std::ostream &outStream, const State &type);



#endif
