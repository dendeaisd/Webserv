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
  std::string line;
  size_t currentLineNumber = 0;

  loadInvalidContextsAndDirectives();
  while (std::getline(file, line)) {
    currentLineNumber++;
    separateTokenStringsFromLine(line);
    lineNumberAddToLineOfTokens(currentLineNumber);
    identifyTokenLineTypes();
    clearTokenLine();
  }
}

void Tokenization::loadInvalidContextsAndDirectives() {
  std::string path = "include/parseConfigFile/configContextAndDirectives/";
  std::string oneLine;

  std::ifstream context(path + "allInvalidContexts.txt");
  if (context.is_open() == false)
    throw cantOpenFile("allInvalidContexts.txt");

  std::ifstream directives(path + "allInvalidDirectives.txt");
  if (context.is_open() == false)
    throw cantOpenFile("currentLineNumber");
  
  while (std::getline(context, oneLine))
    _invalidContext.insert(oneLine);
  while (std::getline(context, oneLine))
    _invalidDirective.insert(oneLine);
  context.close();
  directives.close();
}

void Tokenization::separateTokenStringsFromLine(std::string &line) {
  std::stringstream stream(line);
  std::string oneStr;

  while (stream >> oneStr) {
    _tokensFromLine.push_back(std::make_unique<TokenNode>());
    _tokensFromLine.back()->_tokenStr = oneStr;
  }
}

void Tokenization::lineNumberAddToLineOfTokens(size_t currentLineNumber) {
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++)
    (*it)->_foundLine = currentLineNumber;
}

void Tokenization::identifyTokenLineTypes() {
  checkAndSetSemikolonInToken();
  removeSemikolonFromToken();
  bracketIdentification();
  contextIdentification();
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

  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    std::cout << "STR: [" << (*it)->_tokenStr << "]" << std::endl;
    std::cout << "line number: [" << (*it)->_foundLine << "]\n";
    std::cout << "semicolon set: [" << (*it)->_semikolonSet << "]\n";
    std::cout << "type: [" << (*it)->_type << "]\n\n";
  }
}

bool Tokenization::isInvalidContext(const std::string &context) {
  if (_invalidContext.find(context) != _invalidContext.end())
    return (true);
  return (false);
}
