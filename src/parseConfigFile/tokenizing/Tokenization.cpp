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
  do {
    line = getNextLine(file);
    separateTokenStringsFromLine(line);
  } while (line.empty() == false);
}

std::string Tokenization::getNextLine(std::ifstream &file) {
  std::string line;
  std::getline(file, line);
  return (line);
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
  for (auto it : _tokensFromLine)
    std::cout << "[" << it << "]"<<  std::endl;
}
