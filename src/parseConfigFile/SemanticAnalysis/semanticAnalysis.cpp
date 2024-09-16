/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SemanticAnalysis.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ramymoussa <ramymoussa@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/10 12:06:57 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/16 16:08:02 by fgabler          ###   ########.fr       */
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
  if (OneTokenInLineIsADirective() == false && bracketInLineOfTokens() == false)
    return;

  if (httpValidLine() == true)
    _state = State::HTTPS_CONTEXT;
  else if (serverValidLine() == true)
    _state = State::SERVER_CONTEXT;
  else if (locationValidLine() == true)
    _state = State::LOCATION_CONTEXT;
  else if (backSwitchState(State::HTTPS_CONTEXT,
                           EBracketStatus::HTTP_BRACKET) == true)
    _state = State::MAIN_CONTEXT;
  else if (backSwitchState(State::SERVER_CONTEXT,
                           EBracketStatus::SERVER_BRACKET) == true)
    _state = State::HTTPS_CONTEXT;
  else if (backSwitchState(State::LOCATION_CONTEXT,
                           EBracketStatus::LOCATION_BRACKET) == true)
    _state = State::SERVER_CONTEXT;
  else
    throw InvalidDirective(_tokenLine.front()->_foundLine + ": " +
                           currentLine());
}

bool SemanticAnalysis::OneTokenInLineIsADirective() noexcept {
  for (auto it = _tokenLine.begin(); it != _tokenLine.end(); it++) {
    if ((*it)->_type == TypeToken::HTTP)
      return (true);
    else if ((*it)->_type == TypeToken::SERVER)
      return (true);
    else if ((*it)->_type == TypeToken::LOCATION)
      return (true);
  }
  return (false);
}

bool SemanticAnalysis::httpValidLine() noexcept {
  if (_state == State::MAIN_CONTEXT &&
      _tokenLine.front()->_type == TypeToken::HTTP && _tokenLine.size() == 2 &&
      _tokenLine.back()->_type == TypeToken::OPEN_BRACKET)
    return (true);
  return (false);
}

bool SemanticAnalysis::serverValidLine() noexcept {
  if (_state == State::HTTPS_CONTEXT &&
      _tokenLine.front()->_type == TypeToken::SERVER &&
      _tokenLine.size() == 2 &&
      _tokenLine.back()->_type == TypeToken::OPEN_BRACKET)
    return (true);
  return (false);
}

bool SemanticAnalysis::locationValidLine() noexcept {
  if (_state == State::SERVER_CONTEXT &&
      _tokenLine.front()->_type == TypeToken::LOCATION &&
      _tokenLine.size() == 3 &&
      _tokenLine.back()->_type == TypeToken::OPEN_BRACKET &&
      _tokenLine[1]->_type == TypeToken::URL_LOCATION)
    return (true);
  return (false);
}

bool SemanticAnalysis::backSwitchState(State state,
                                       EBracketStatus bracket) noexcept {
  if (_state == state && _bracketStatus[bracket].size() == 0 &&
      validClosingBracket() == true)
    return (true);
  return (false);
}

void SemanticAnalysis::trackBrackets() {
  if (bracketInLineOfTokens() == false) return;
  EBracketStatus status = getCurrentBracketStatus();

  if (validDirectiveLine() == true)
    _bracketStatus[status].push('{');
  else if (validClosingBracket() && _bracketStatus[status].size() > 0)
    _bracketStatus[status].pop();
  else
    throw InvalidBracket(_tokenLine.front()->_foundLine + ": " + currentLine());
}

bool SemanticAnalysis::bracketInLineOfTokens() noexcept {
  for (auto it = _tokenLine.begin(); it != _tokenLine.end(); it++) {
    if ((*it)->_type == TypeToken::OPEN_BRACKET)
      return (true);
    else if ((*it)->_type == TypeToken::CLOSING_BRACKET)
      return (true);
  }
  return (false);
}

EBracketStatus SemanticAnalysis::getCurrentBracketStatus() noexcept {
  if (openBracketStateIs(TypeToken::HTTP, State::MAIN_CONTEXT) == true)
    return (HTTP_BRACKET);
  else if (openBracketStateIs(TypeToken::SERVER, State::HTTPS_CONTEXT) == true)
    return (SERVER_BRACKET);
  else if (openBracketStateIs(TypeToken::LOCATION, State::SERVER_CONTEXT) ==
           true)
    return (LOCATION_BRACKET);
  else if (moveStateBackFrom(State::HTTPS_CONTEXT) == true)
    return (HTTP_BRACKET);
  else if (moveStateBackFrom(State::SERVER_CONTEXT) == true)
    return (SERVER_BRACKET);
  else if (moveStateBackFrom(State::LOCATION_CONTEXT) == true)
    return (LOCATION_BRACKET);
  return (MAIN_BRACKET);
}

bool SemanticAnalysis::validDirectiveLine() noexcept {
  if (httpValidLine() == true)
    return (true);
  else if (serverValidLine() == true)
    return (true);
  else if (locationValidLine() == true)
    return (true);
  return (false);
}

bool SemanticAnalysis::openBracketStateIs(TypeToken expectedType,
                                          State currentState) noexcept {
  if (_state == currentState && _tokenLine.front()->_type == expectedType &&
      validDirectiveLine())
    return (true);
  return (false);
}

bool SemanticAnalysis::moveStateBackFrom(State currentState) noexcept {
  if (_state == currentState && validClosingBracket() == true) return (true);
  return (false);
}

bool SemanticAnalysis::validClosingBracket() noexcept {
  if (_tokenLine.size() == 1 &&
      _tokenLine.front()->_type == TypeToken::CLOSING_BRACKET)
    return (true);
  return (false);
}

std::string SemanticAnalysis::currentLine() {
  std::string currentStr;
  for (auto it = _tokenLine.begin(); it != _tokenLine.end(); it++) {
    if (it != (_tokenLine.end() - 1))
      currentStr + (*it)->_tokenStr + " ";
    else 
      currentStr + (*it)->_tokenStr;
  }
  return (currentStr);
}

std::ostream &operator<<(std::ostream &outStream, const State &type) {
  if (type == State::MAIN_CONTEXT)
    outStream << "MAIN_CONTEXT    ";
  else if (type == State::HTTPS_CONTEXT)
    outStream << "HTTPS_CONTEXT   ";
  else if (type == State::SERVER_CONTEXT)
    outStream << "SERVER_CONTEXT  ";
  else if (type == State::LOCATION_CONTEXT)
    outStream << "LOCATION_CONTEXT";
  return (outStream);
}
