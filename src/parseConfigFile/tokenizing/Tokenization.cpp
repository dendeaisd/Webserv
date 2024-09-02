#include "Tokenization.hpp"

#include <cctype>
#include <fstream>
#include <string>
#include <vector>

Tokenization::Tokenization(std::ifstream &file) {
  std::string line;
  std::vector<std::string> seperaatedTokensStr;
  do {
    line = getNextLine(file);

  } while (line.empty() == false);
}

std::string Tokenization::getNextLine(std::ifstream &file) {
  std::string line;
  std::getline(file, line);
  return (line);
}

void Tokenization::seperateTokenStringsFromLine(
    std::string &line, std::vector<std::string> &seperatedTokensStr) {}

std::string Tokenization::getNextTokenString(std::string &line) {
}
