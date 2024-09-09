#include "Tokenization.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ExceptionsTokenization.hpp"

Tokenization::Tokenization(std::ifstream &file) {
  unsigned currentLineNumber = 0;

  loadInvalidContextsAndDirectives();
  while (std::getline(file, _line)) {
    currentLineNumber++;
    removeCommentsFromLine();
    separateTokenStringsFromLine();
    lineNumberAddToLineOfTokens(currentLineNumber);
    identifyTokenLineTypes();
    addToLineTokensToTokenChain();
    clearTokenLine();
  }
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
  if (file.is_open() == false) throw cantOpenFile(filePath);
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
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    size_t pos = (*it)->_tokenStr.size() - 1;
    if ((*it)->_tokenStr[pos] == ';') (*it)->_semikolonSet = true;
  }
}

void Tokenization::removeSemikolonFromToken() {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    size_t pos = (*it)->_tokenStr.size() - 1;
    if ((*it)->_semikolonSet == true) (*it)->_tokenStr.erase(pos, 1);
  }
}

void Tokenization::clearTokenLine() { _tokensFromLine.clear(); }

void Tokenization::bracketIdentification() {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    if ((*it)->_tokenStr.find('{') != std::string::npos &&
        (*it)->_tokenStr.size() == 1)
      (*it)->_type = TypeToken::OPEN_BRACKET;
    else if ((*it)->_tokenStr.find('}') != std::string::npos &&
             (*it)->_tokenStr.size() == 1)
      (*it)->_type = TypeToken::OPEN_BRACKET;
    else if (((*it)->_tokenStr.find('{') != std::string::npos ||
              (*it)->_tokenStr.find('}') != std::string::npos) &&
             (*it)->_tokenStr.size() != 1)
      throw invalidFormat("Breacktets neeed to be seperated by spaces");
  }
}

void Tokenization::contextIdentification() {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    if ((*it)->_tokenStr == "http")
      (*it)->_type = TypeToken::HTTP;
    else if ((*it)->_tokenStr == "server")
      (*it)->_type = TypeToken::SERVER;
    else if ((*it)->_tokenStr == "location")
      (*it)->_type = TypeToken::LOCATION;
    else if (isInvalidContext((*it)->_tokenStr) == true)
      throw invalidContext((*it)->_tokenStr + " context is not implemented");
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
      throw invalidDirective((*it)->_tokenStr);
    else if ((*it)->_tokenStr == "ssl_certificate")
      (*it)->_type = TypeToken::SSL_CERTIFICATE;
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
    else if ((*it)->_tokenStr == "prox_pass")
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
  }
}

void Tokenization::valueIdentification() {
  if (_tokensFromLine.empty() ||
      validDirective(_tokensFromLine[0]->_tokenStr) == false)
    return;

  if (_tokensFromLine.size() < 2 &&
      _tokensFromLine.back()->_semikolonSet == false)
    throw invalidFormat("semikolon not set at end of line " +
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
    throw invalidFormat("Location is set incorrect. Line: " + _line);
  } else if (_tokensFromLine[1]->_type == TypeToken::DEFAULT)
    _tokensFromLine[1]->_type = TypeToken::URL_LOCATION;
}

void Tokenization::addToLineTokensToTokenChain() {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    _chainOfTokens.push_back(std::make_unique<TokenNode>(**it));
  }
}

void Tokenization::printTokens() {

  for (auto it = _chainOfTokens.begin(); it != _chainOfTokens.end(); it++) {
    std::cout << "STR: [" << (*it)->_tokenStr << "]" << std::endl;
    std::cout << "line number: [" << (*it)->_foundLine << "]\n";
    std::cout << "semicolon set: [" << (*it)->_semikolonSet << "]\n";
    std::cout << "type: [" << (*it)->_type << "]\n\n";
  }
}
