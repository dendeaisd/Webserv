#ifndef NODE_TOKEN_HPP
#define NODE_TOKEN_HPP

#include <string>
#include "TypeToken.hpp"

class TokenNode {
 public:
  TokenNode() : _foundLine(0), _type(TypeToken::DEFAULT) {}

  std::string _tokenStr;
  TypeToken _type;
  uint64_t _foundLine;
};

#endif
