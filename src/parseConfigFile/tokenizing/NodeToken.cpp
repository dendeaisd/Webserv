/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NodeToken.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:20:25 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/10 09:27:20 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "NodeToken.hpp"

TokenNode::TokenNode()
    : _type(TypeToken::DEFAULT), _foundLine("0"), _semikolonSet(false) {}

TokenNode::TokenNode(const TokenNode &other)
    : _tokenStr(other._tokenStr),
      _type(other._type),
      _foundLine(other._foundLine),
      _semikolonSet(other._semikolonSet) {}

TokenNode &TokenNode::operator=(const TokenNode &other) {
  if (this != &other) {
    _tokenStr = other._tokenStr;
    _type = other._type;
    _foundLine = other._foundLine;
    _semikolonSet = other._semikolonSet;
  }
  return (*this);
}
