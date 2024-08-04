#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum struct ETypesToken;

typedef struct STokenNode {
  ETypesToken tokenTypes_;
  std::string strToken_;
  unsigned int lineNumber_;
} TTokenNode;

enum struct ETypesToken {
  KEYWORD = 0,
  PARAMETER = 1,
  OPERATOR = 2
};

#endif
