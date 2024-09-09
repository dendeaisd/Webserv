#ifndef NODE_TOKEN_HPP
#define NODE_TOKEN_HPP

#include <string>

#include "TypeToken.hpp"

class TokenNode {
 public:
  TokenNode();
  TokenNode(const TokenNode &other);
  TokenNode &operator=(const TokenNode &other);

  std::string _tokenStr;
  TypeToken _type;
  std::string _foundLine;
  bool _semikolonSet;
};

#endif
