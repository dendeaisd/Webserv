#ifndef NODE_TOKEN_HPP
#define NODE_TOKEN_HPP

#include <string>

#include "TypeToken.hpp"

class TokenNode {
 public:
  TokenNode();

  std::string _tokenStr;
  TypeToken _type;
  size_t _foundLine;
  bool _semikolonSet;

};

#endif
