#ifndef SEMANTIC_ANALYSIS_HPP
#define SEMANTIC_ANALYSIS_HPP

#include <iostream>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <vector>

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
  ~SemanticAnalysis() {};

  std::unique_ptr<ConfigFile> getConfigFile();

 private:
  void preSetup();
  void loadDirectives();
  void openFile(const std::string &filePath, std::ifstream &file);
  void fillWithFileContent(std::set<std::string> &, std::ifstream &);

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

  void saveDirectiveValue();
  void mainDirectiveSave();
  void httpSaveDirective();
  void serverSaveDirective();
  void locationSaveDirective();

  template <typename T>
  void saveMultipleDirectiveValue(std::vector<T> &) noexcept;

  bool isDirectiveInLine(std::set<std::string> &) noexcept;
  bool isValidMainDirective() noexcept;
  bool isValidHttpDirective() noexcept;
  bool multipleValueValidDirective(
      const std::set<std::string> &) const noexcept;
  bool isValidSingleValueDirective(
      const std::set<std::string> &) const noexcept;
  bool canDirectiveBeSaved(TypeToken) const noexcept;
  bool serverValidDirective() const noexcept;
  bool locationValidDirective() const noexcept;
  bool isValueEmpty(TypeToken) const noexcept;

  bool validListen() const noexcept;
  std::string getIpListenValue(std::string &ipAndPort) const noexcept;
  bool validIpChar(char c) const noexcept;
  int getPortListenValue(std::string &ipAndPort) const noexcept;
  void saveListenValue() const noexcept;
  void listenSetInServerCheck() const;

  void setClientMaxBodySize();
  size_t convertMaxBodySize(const std::string &value) const;
  size_t getMaxBodySizeMultiplier(char c) const;
  bool canClientMaxBodySetBeSet() const noexcept;
  bool isClientMaxBodySizeSet() const noexcept;

  bool locationReturned() const noexcept;
  void saveReturnValue();
  int convertStatusCode(std::string &) const;
  bool isWithInStatusCodeRange(int statusCode) const noexcept;
  bool isValidThreeHundredStatusRange(int statusCode) const noexcept;
  bool isValidReturnURL() const noexcept;
  std::string getReturnMessage() const noexcept;

  void saveRequestTimeOut();

  std::string getThrowMessage() noexcept;
  std::string getThrowMessage() const noexcept;
  std::string currentLine() noexcept;
  std::string currentLine() const noexcept;

  std::unique_ptr<ConfigFile> _config;

  State _state;
  bool _httpAlreadySet;
  std::stack<char> _bracketStatus[4];

  std::set<std::string> _mainValidDirective;
  std::set<std::string> _httpValidDirective;
  std::set<std::string> _httpMultipleValueValidDirective;
  std::set<std::string> _httpSingleValueValidDirective;
  std::set<std::string> _serverValidDirective;
  std::set<std::string> _serverMultiValidDirective;
  std::set<std::string> _serverSingleValDirec;
  std::set<std::string> _locationValidDirective;
  std::set<std::string> _multiLocationValDirec;
  std::set<std::string> _singleLocationValDirec;

  std::vector<std::unique_ptr<TokenNode>> _tokenLine;
};

#include "SemanticAnalysis.tpp"

std::ostream &operator<<(std::ostream &outStream, const State &type);

#endif
