/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semanticAnalysis.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ramymoussa <ramymoussa@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/10 12:06:57 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/12 14:48:46 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SemanticAnalysis.hpp"

#include <string.h>

#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "ExceptionsParsing.hpp"
#include "HttpContext.hpp"
#include "ServerContext.hpp"

SemanticAnalysis::SemanticAnalysis(TokenStructure &token) {
  _state = State::MAIN_CONTEXT;

  for (auto it = token.begin(); it != token.end(); it++) {
    _tokenLine = std::move((*it));
    trackBrackets();
    setCurrentState();
  }
}

SemanticAnalysis::~SemanticAnalysis() {}

void SemanticAnalysis::setCurrentState() {
  if (_state == StoringStates::MAIN_CONTEXT && (*_it)->_type == TypeToken::HTTP)
    directiveStateSetSave(StoringStates::HTTPS_CONTEXT);
  else if (_state == StoringStates::HTTPS_CONTEXT &&
           (*_it)->_type == TypeToken::SERVER)
    directiveStateSetSave(StoringStates::SERVER_CONTEXT);
  /* else if (_state == StoringStates::SERVER_CONTEXT &&
           (*_it)->_type == TypeToken::LOCATION)
    directiveStateSetSave(StoringStates::LOCATION_CONTEXT); */
  else if (_state == StoringStates::HTTPS_CONTEXT &&
           _bracketStatus[HTTP_BRACKET].size() == 0)
    _state = StoringStates::MAIN_CONTEXT;
  else if (_state == StoringStates::SERVER_CONTEXT &&
           _bracketStatus[SERVER_BRACKET].size() == 0)
    _state = StoringStates::HTTPS_CONTEXT;
  else if (_state == StoringStates::LOCATION_CONTEXT &&
           _bracketStatus[LOCATION_BRACKET].size() == 0)
    _state = StoringStates::SERVER_CONTEXT;
}

void SemanticAnalysis::directiveStateSetSave(const StoringStates &state) {
  if ((*_it)->_type == TypeToken::LOCATION) {
    moveToNextTokenSave();
  }
  
  moveToNextTokenSave();
  if ((*_it)->_type != TypeToken::OPEN_BRACKET)
    throw MissingSymbol((*_it)->_foundLine + ": " + (*_it)->_tokenStr);
  _state = state;
  _tokenIsHandled = true;
}

void SemanticAnalysis::moveToNextTokenSave() {
  if (_it != _tokens.end() && (_it + 1) != _tokens.end())
    _it++;
  else
    throw MissingSymbol((*_it)->_foundLine + ": " + (*_it)->_tokenStr);
}

void SemanticAnalysis::trackBrackets() {
  EBracketStatus status = getCurrentBracketStatus();
  if ((*_it)->_type == TypeToken::OPEN_BRACKET && _tokenIsHandled == true)
    _bracketStatus[status].push('{');
  else if ((*_it)->_type == TypeToken::CLOSING_BRACKET &&
           _bracketStatus[status].size() > 0)
    _bracketStatus[status].pop();
  else if ((*_it)->_type == TypeToken::CLOSING_BRACKET &&
           _bracketStatus[status].size() > 0)
    throw NoOpeningBracketFound((*_it)->_foundLine + ": " + (*_it)->_tokenStr);
}

EBracketStatus SemanticAnalysis::getCurrentBracketStatus() {
  switch (_state) {
    case StoringStates::MAIN_CONTEXT:
      return (MAIN_BRACKET);
    case StoringStates::HTTPS_CONTEXT:
      return (HTTP_BRACKET);
    case StoringStates::SERVER_CONTEXT:
      return (SERVER_BRACKET);
    case StoringStates::LOCATION_CONTEXT:
      return (LOCATION_BRACKET);
      break;
  }
  return (MAIN_BRACKET);
}
