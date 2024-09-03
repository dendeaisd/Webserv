#include "Tokenization.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

Tokenization::Tokenization(std::ifstream &file) {
  std::string line;
  size_t currentLineNumber = 0;

  while (std::getline(file, line)) {
    currentLineNumber++;
    separateTokenStringsFromLine(line);
    lineNumberAddToLineOfTokens(currentLineNumber);
    identifyTokenLineTypes();
    clearTokenLine();
  }
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
    /*
    else if ((*it)->_tokenStr.find('}') != std::string::npos &&
             (*it)->_tokenStr.size() > 1)
      // trow exception
    */
  }
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++) {
    std::cout << "STR: [" << (*it)->_tokenStr << "]" << std::endl;
    std::cout << "line number: [" << (*it)->_foundLine << "]\n";
    std::cout << "semicolon set: [" << (*it)->_semikolonSet << "]\n";
    std::cout << "type: [" << (*it)->_type << "]\n\n";
  }
}
