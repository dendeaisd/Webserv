#include "Tokenization.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "ExceptionsParsing.hpp"

Tokenization::Tokenization(std::ifstream &file) {
  unsigned currentLineNumber = 0;

  loadInvalidContextsAndDirectives();
  while (std::getline(file, _line)) {
    currentLineNumber++;
    removeCommentsFromLine();
    separateTokenStringsFromLine();
    lineNumberAddToLineOfTokens(currentLineNumber);
    identifyTokenLineTypes();
    trackBreackets();
    invalidCharacterCheck();
    undefinedTokenCheck();
    addToLineTokensToDoubleVectorOfTokens();
    clearTokenLine();
  }
  leftBracketsCheck();
}

void Tokenization::loadInvalidContextsAndDirectives() {
  std::string path = "include/parseConfigFile/configContextAndDirectives/";

  std::ifstream invalContext;
  std::ifstream invalDirectives;
  std::ifstream valDirective;

  openFile((path + "allInvalidContexts.txt"), invalContext);
  openFile((path + "allInvalidDirectives.txt"), invalDirectives);
  openFile((path + "validDirectives.txt"), valDirective);

  std::string oneLine;
  while (std::getline(invalContext, oneLine)) _invalidContext.insert(oneLine);
  while (std::getline(invalDirectives, oneLine))
    _invalidDirective.insert(oneLine);
  while (std::getline(valDirective, oneLine)) _directiveValid.insert(oneLine);

  invalContext.close();
  invalDirectives.close();
  valDirective.close();
}

void Tokenization::openFile(const std::string &filePath, std::ifstream &file) {
  file.open(filePath);
  if (file.is_open() == false) throw CantOpenFile(filePath);
}

void Tokenization::removeCommentsFromLine() {
  if (_line.find('#') == std::string::npos) return;
  size_t pos = _line.find('#');
  _line.erase(pos, (_line.size() - pos));
}

void Tokenization::separateTokenStringsFromLine() {
  std::stringstream stream(_line);
  std::string oneStr;

  while (stream >> oneStr) {
    _tokensFromLine.push_back(std::make_unique<TokenNode>());
    _tokensFromLine.back()->_tokenStr = oneStr;
  }
}

void Tokenization::lineNumberAddToLineOfTokens(unsigned int currentLineNumber) {
  std::string lineNumberStr = std::to_string(currentLineNumber);

  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++)
    (*it)->_foundLine = lineNumberStr;
}

void Tokenization::identifyTokenLineTypes() {
  checkAndSetSemikolonInToken();
  removeSemikolonFromToken();
  bracketIdentification();
  contextIdentification();
  directiveIdentification();
  locationUrlIdentification();
  valueIdentification();
}

void Tokenization::checkAndSetSemikolonInToken() {
  if (_tokensFromLine.size() < 1) return;
  size_t pos = _tokensFromLine.back()->_tokenStr.size() - 1;
  if (validDirective(_tokensFromLine.front()->_tokenStr) == true &&
      _tokensFromLine.back()->_tokenStr[pos] == ';')
    _tokensFromLine.back()->_semikolonSet = true;
  else if (validDirective(_tokensFromLine.front()->_tokenStr) == true &&
           _tokensFromLine.back()->_tokenStr[pos] != ';')
    throw DirectiveValueNotTerminatedWithSemicolon(
        _tokensFromLine.front()->_foundLine + " :" + _line);
}

void Tokenization::removeSemikolonFromToken() {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    size_t pos = (*it)->_tokenStr.size() - 1;
    if ((*it)->_semikolonSet == true) (*it)->_tokenStr.erase(pos, 1);
  }
}

void Tokenization::bracketIdentification() {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    if ((*it)->_tokenStr[0] == '{')
      (*it)->_type = TypeToken::OPEN_BRACKET;
    else if ((*it)->_tokenStr[0] == '}')
      (*it)->_type = TypeToken::CLOSING_BRACKET;
    else if (((*it)->_tokenStr.find('{') != std::string::npos ||
              (*it)->_tokenStr.find('}') != std::string::npos) &&
             (*it)->_tokenStr.size() != 1)
      throw InvalidFormat("Breacktets neeed to be seperated by spaces");
  }
}

void Tokenization::clearTokenLine() { _tokensFromLine.clear(); }

void Tokenization::contextIdentification() {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    if (isInvalidContext((*it)->_tokenStr) == true)
      throw InvalidContext((*it)->_tokenStr +
                           " context is not implemented: " + _line);
    else if ((*it)->_tokenStr == "http")
      (*it)->_type = TypeToken::HTTP;
    else if ((*it)->_tokenStr == "server")
      (*it)->_type = TypeToken::SERVER;
    else if ((*it)->_tokenStr == "location")
      (*it)->_type = TypeToken::LOCATION;
  }
}

bool Tokenization::isInvalidContext(const std::string &context) {
  if (_invalidContext.find(context) != _invalidContext.end()) return (true);
  return (false);
}

bool Tokenization::isInvalidDirective(const std::string &directive) {
  if (_invalidDirective.find(directive) != _invalidDirective.end())
    return (true);
  return (false);
}

void Tokenization::directiveIdentification() {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    if (isInvalidDirective((*it)->_tokenStr) == true)
      throw InvalidDirective((*it)->_foundLine + ": " + _line);
    else if ((*it)->_tokenStr == "worker_processes")
      (*it)->_type = TypeToken::WORKER_PROCESS;
    else if ((*it)->_tokenStr == "pid")
      (*it)->_type = TypeToken::PID;
    else if ((*it)->_tokenStr == "ssl_certificate")
      (*it)->_type = TypeToken::SSL_CERTIFICATE;
    else if ((*it)->_tokenStr == "error_log")
      (*it)->_type = TypeToken::ERROR_LOG;
    else if ((*it)->_tokenStr == "ssl_certificate_key")
      (*it)->_type = TypeToken::SSL_CERTIFICATE_KEY;
    else if ((*it)->_tokenStr == "index")
      (*it)->_type = TypeToken::INDEX;
    else if ((*it)->_tokenStr == "root")
      (*it)->_type = TypeToken::ROOT;
    else if ((*it)->_tokenStr == "listen")
      (*it)->_type = TypeToken::LISTEN;
    else if ((*it)->_tokenStr == "geoip_country")
      (*it)->_type = TypeToken::GEO_IP_COUNTRY;
    else if ((*it)->_tokenStr == "proxy_cache_path")
      (*it)->_type = TypeToken::PROXY_CACHE_PATH;
    else if ((*it)->_tokenStr == "proxy_cache")
      (*it)->_type = TypeToken::PROXY_CACHE;
    else if ((*it)->_tokenStr == "proxy_cache_use_stale")
      (*it)->_type = TypeToken::PROXY_CACHE_USE_STALE;
    else if ((*it)->_tokenStr == "gzip")
      (*it)->_type = TypeToken::GZIP;
    else if ((*it)->_tokenStr == "gzip_types")
      (*it)->_type = TypeToken::GZIP_TYPES;
    else if ((*it)->_tokenStr == "limit_req_zone")
      (*it)->_type = TypeToken::LIMIT_RED_ZONE;
    else if ((*it)->_tokenStr == "proxy_set_header")
      (*it)->_type = TypeToken::LIMIT_RED_ZONE;
    else if ((*it)->_tokenStr == "proxy_cache_valid")
      (*it)->_type = TypeToken::PROXY_CACHE_VALID;
    else if ((*it)->_tokenStr == "server_name")
      (*it)->_type = TypeToken::SERVER_NAME;
    else if ((*it)->_tokenStr == "ssl_certificate")
      (*it)->_type = TypeToken::SSL_CERTIFICATE;
    else if ((*it)->_tokenStr == "ssl_certificate_key")
      (*it)->_type = TypeToken::SSL_CERTIFICATE_KEY;
    else if ((*it)->_tokenStr == "include")
      (*it)->_type = TypeToken::INCLUDE;
    else if ((*it)->_tokenStr == "proxy_pass")
      (*it)->_type = TypeToken::PROXY_PASS;
    else if ((*it)->_tokenStr == "alias")
      (*it)->_type = TypeToken::ALIAS;
    else if ((*it)->_tokenStr == "try_files")
      (*it)->_type = TypeToken::TRY_FILES;
    else if ((*it)->_tokenStr == "error_page")
      (*it)->_type = TypeToken::ERROR_PAGE;
    else if ((*it)->_tokenStr == "access_log")
      (*it)->_type = TypeToken::ACCESS_LOG;
    else if ((*it)->_tokenStr == "deny")
      (*it)->_type = TypeToken::DENY;
    else if ((*it)->_tokenStr == "cgi")
      (*it)->_type = TypeToken::CGI;
    else if ((*it)->_tokenStr == "rewrite")
      (*it)->_type = TypeToken::REWRITE;
    else if ((*it)->_tokenStr == "client_max_body_size")
      (*it)->_type = TypeToken::CLIENT_MAX_BODY_SIZE;
    else if ((*it)->_tokenStr == "autoindex")
      (*it)->_type = TypeToken::AUTO_INDEX;
    else if ((*it)->_tokenStr == "allow_methods")
      (*it)->_type = TypeToken::ALLOW_METHODS;
    else if ((*it)->_tokenStr == "return")
      (*it)->_type = TypeToken::RETURN;
    else if ((*it)->_tokenStr == "upload_dir")
      (*it)->_type = TypeToken::UPLOAD_DIR;
    else if ((*it)->_tokenStr == "request_timeout")
      (*it)->_type = TypeToken::REQUEST_TIMEOUT;
  }
}

void Tokenization::valueIdentification() {
  if (_tokensFromLine.empty() ||
      validDirective(_tokensFromLine[0]->_tokenStr) == false)
    return;

  if (_tokensFromLine.size() < 2 &&
      _tokensFromLine.back()->_semikolonSet == false)
    throw InvalidFormat("semikolon not set at end of line " +
                        _tokensFromLine.back()->_tokenStr);

  for (auto it = _tokensFromLine.begin() + 1; it != _tokensFromLine.end();
       it++) {
    if ((*it)->_type == TypeToken::DEFAULT) (*it)->_type = TypeToken::VALUE;
    if ((*it)->_semikolonSet == true) return;
  }
}

bool Tokenization::validDirective(std::string &directive) {
  if (_directiveValid.find(directive) != _directiveValid.end()) return (true);
  return (false);
}

void Tokenization::locationUrlIdentification() {
  if (_tokensFromLine.empty() ||
      _tokensFromLine.front()->_type != TypeToken::LOCATION)
    return;
  else if (_tokensFromLine.size() < 2 || _tokensFromLine.size() > 3) {
    throw InvalidFormat("Location is set incorrect. Line: " + _line);
  } else if (_tokensFromLine[1]->_type == TypeToken::DEFAULT)
    _tokensFromLine[1]->_type = TypeToken::URL_LOCATION;
}

void Tokenization::trackBreackets() {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    if ((*it)->_type == TypeToken::OPEN_BRACKET)
      _bracketCount.push('{');
    else if ((*it)->_type == TypeToken::CLOSING_BRACKET &&
             _bracketCount.size() > 0)
      _bracketCount.pop();
    else if ((*it)->_type == TypeToken::CLOSING_BRACKET &&
             _bracketCount.size() == 0)
      throw NoOpeningBracketFound((*it)->_foundLine + ": " + _line);
  }
}

void Tokenization::invalidCharacterCheck() const {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    for (int i = 0; (*it)->_tokenStr[i] != '\0'; i++) {
      if (std::isalnum((*it)->_tokenStr[i]) == false &&
          isValidSpecialCharacter((*it)->_tokenStr[i]) == false)
        throw InvalidCharacterFound(_tokensFromLine.front()->_foundLine + ": " +
                                    _line);
    }
  }
}

bool Tokenization::isValidSpecialCharacter(char c) const {
  if (c == ';' || c == '{' || c == '}' || c == '#' || c == '.' || c == '/' ||
      c == ':' || c == '=' || c == '-' || c == '_' || c == '?' || c == '&' ||
      c == '+')
    return (true);
  return (false);
}

void Tokenization::addToLineTokensToDoubleVectorOfTokens() {
  if (_tokensFromLine.size() > 0)
    _doubleVectorOfTokens.push_back(std::move(_tokensFromLine));
}

void Tokenization::printTokens() {
  for (auto it = _doubleVectorOfTokens.begin();
       it != _doubleVectorOfTokens.end(); it++) {
    for (auto iter = (*it).begin(); iter != (*it).end(); iter++) {
      std::cout << "STR: [" << (*iter)->_tokenStr << "]" << std::endl;
      std::cout << "line number: [" << (*iter)->_foundLine << "]\n";
      std::cout << "semicolon set: [" << (*iter)->_semikolonSet << "]\n";
      std::cout << "type: [" << (*iter)->_type << "]\n\n";
    }
  }
}

std::vector<std::vector<std::unique_ptr<TokenNode>>> Tokenization::getTokens() {
  if (_doubleVectorOfTokens.empty()) throw EmptyVectorOfTokens();
  return (std::move(_doubleVectorOfTokens));
}

void Tokenization::leftBracketsCheck() {
  if (_bracketCount.size() != 0) throw NoClosingBracketFound();
}

void Tokenization::undefinedTokenCheck() {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    if ((*it)->_type == TypeToken::DEFAULT)
      throw CantIdentifySetting((*it)->_foundLine + ": " + (*it)->_tokenStr);
  }
}
