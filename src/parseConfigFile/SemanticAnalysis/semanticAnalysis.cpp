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

  if (canEnterHttpContext() == true && _httpAlreadySet == false) {
    _state = State::HTTPS_CONTEXT;
    _httpAlreadySet = true;
  } else if (canEnterServerContext() == true)
    _state = State::SERVER_CONTEXT;
  else if (canEnterLocationContext() == true)
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

bool SemanticAnalysis::canEnterHttpContext() noexcept {
  if (_state == State::MAIN_CONTEXT &&
      _tokenLine.front()->_type == TypeToken::HTTP && _tokenLine.size() == 2 &&
      _tokenLine.back()->_type == TypeToken::OPEN_BRACKET)
    return (true);
  return (false);
}

bool SemanticAnalysis::canEnterServerContext() noexcept {
  if (_state == State::HTTPS_CONTEXT &&
      _tokenLine.front()->_type == TypeToken::SERVER &&
      _tokenLine.size() == 2 &&
      _tokenLine.back()->_type == TypeToken::OPEN_BRACKET)
    return (true);
  return (false);
}

bool SemanticAnalysis::canEnterLocationContext() noexcept {
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
  if (status == ERROR) throw InvalidDirective(getThrowMessage());

  if (canTransitionToNewContext() == true)
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
  return (ERROR);
}

bool SemanticAnalysis::canTransitionToNewContext() noexcept {
  if (canEnterHttpContext() == true)
    return (true);
  else if (canEnterServerContext() == true)
    return (true);
  else if (canEnterLocationContext() == true)
    return (true);
  return (false);
}

bool SemanticAnalysis::openBracketStateIs(TypeToken expectedType,
                                          State currentState) noexcept {
  if (_state == currentState && _tokenLine.front()->_type == expectedType &&
      canTransitionToNewContext())
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

void SemanticAnalysis::possibleCreationOfNewContext() noexcept {
  if (validLocationLine() == true)
    createLocationContext();
  else if (validServerLine() == true)
    createServerContext();
}

void SemanticAnalysis::createServerContext() noexcept {
  _config._httpContext._serverContext.push_back(
      std::make_unique<ServerContext>());
  // std::cout << "NEW SERVER\n";
}

void SemanticAnalysis::createLocationContext() noexcept {
  _config._httpContext._serverContext.back()->_locationContext.push_back(
      std::make_unique<Location>());

  _config._httpContext._serverContext.back()
      ->_locationContext.back()
      ->_urlValue = _tokenLine[1]->_tokenStr;
  // std::cout << "NEW LOCATION\n";
}

bool SemanticAnalysis::validServerLine() noexcept {
  if (_state == State::SERVER_CONTEXT && _tokenLine.size() == 2 &&
      _tokenLine.front()->_type == TypeToken::SERVER &&
      _tokenLine.back()->_type == TypeToken::OPEN_BRACKET)
    return (true);
  return (false);
}

bool SemanticAnalysis::validLocationLine() noexcept {
  if (_state == State::LOCATION_CONTEXT && _tokenLine.size() == 3 &&
      _tokenLine.front()->_type == TypeToken::LOCATION &&
      _tokenLine.back()->_type == TypeToken::OPEN_BRACKET &&
      _tokenLine[1]->_type == TypeToken::URL_LOCATION)
    return (true);
  return (false);
}

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
