#include "Tokenization.hpp"

#include <cctype>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <memory>

Tokenization::Tokenization(std::ifstream &file) {
  std::string line;
  std::vector<std::string> seperaatedTokensStr;
  while (std::getline(file, line)) {
    separateTokenStringsFromLine(line);
  }
}

void Tokenization::separateTokenStringsFromLine(std::string &line)
{
  std::stringstream stream(line);
  std::string oneStr;
  
  while (stream >> oneStr)
  {
    _tokensFromLine.push_back(std::make_unique<TokenNode>());
    _tokensFromLine.back()->_tokenStr = oneStr;
  }
  for (auto it = _tokensFromLine.begin(); it != _tokensFromLine.end(); it++)
    std::cout << "[" << (*it)->_tokenStr << "]"<<  std::endl;
}
