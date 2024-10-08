/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semanticAnalysis.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ramymoussa <ramymoussa@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/10 12:06:57 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/26 00:45:01 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SemanticAnalysis.hpp"

#include <string.h>

#include <cctype>
#include <cstring>
#include <fstream>
#include <limits>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "ExceptionsParsing.hpp"
#include "HttpContext.hpp"
#include "ServerContext.hpp"

SemanticAnalysis::SemanticAnalysis(TokenStructure &token) {
  preSetup();
  for (auto it = token.begin(); it != token.end(); it++) {
    _tokenLine = std::move((*it));
    trackBrackets();
    setCurrentState();
    possibleCreationOfNewContext();
    saveDirectiveValue();
  }
  listenSetInServerCheck();
}

void SemanticAnalysis::preSetup() {
  _state = State::MAIN_CONTEXT;
  _httpAlreadySet = false;

  _config = std::make_unique<ConfigFile>();
  loadDirectives();
}

void SemanticAnalysis::loadDirectives() {
  std::string path = "include/parseConfigFile/configContextAndDirectives/";

  std::ifstream mainValidDirective;
  std::ifstream httpValidDirective;
  std::ifstream multipleHttpValidDirective;
  std::ifstream singleHttpValidDirective;
  std::ifstream serverValidDirectives;
  std::ifstream multiServerValidDirec;
  std::ifstream singleServerValDirec;
  std::ifstream locationValDirec;
  std::ifstream locationMultiValDirec;
  std::ifstream locationSingleValDirec;

  openFile(path + "mainDirectiveValid.txt", mainValidDirective);
  openFile(path + "httpValidDirectives.txt", httpValidDirective);
  openFile(path + "httpMultiValueDirectives.txt", multipleHttpValidDirective);
  openFile(path + "httpSingleValueDirective.txt", singleHttpValidDirective);
  openFile(path + "serverValidDirectives.txt", serverValidDirectives);
  openFile(path + "serverMultiValueDirective.txt", multiServerValidDirec);
  openFile(path + "serverSingleValDirec.txt", singleServerValDirec);
  openFile(path + "locationValDirec.txt", locationValDirec);
  openFile(path + "locationMultiValDirec.txt", locationMultiValDirec);
  openFile(path + "locationSingleValDirec.txt", locationSingleValDirec);

  fillWithFileContent(_mainValidDirective, mainValidDirective);
  fillWithFileContent(_httpValidDirective, httpValidDirective);
  fillWithFileContent(_httpMultipleValueValidDirective,
                      multipleHttpValidDirective);
  fillWithFileContent(_httpSingleValueValidDirective, singleHttpValidDirective);
  fillWithFileContent(_serverValidDirective, serverValidDirectives);
  fillWithFileContent(_serverMultiValidDirective, multiServerValidDirec);
  fillWithFileContent(_serverSingleValDirec, singleServerValDirec);
  fillWithFileContent(_locationValidDirective, locationValDirec);
  fillWithFileContent(_multiLocationValDirec, locationMultiValDirec);
  fillWithFileContent(_singleLocationValDirec, locationSingleValDirec);

  mainValidDirective.close();
  httpValidDirective.close();
  multipleHttpValidDirective.close();
  singleHttpValidDirective.close();
  serverValidDirectives.close();
  multiServerValidDirec.close();
  singleServerValDirec.close();
  locationValDirec.close();
  locationMultiValDirec.close();
  locationSingleValDirec.close();
}

void SemanticAnalysis::fillWithFileContent(std::set<std::string> &directive,
                                           std::ifstream &file) {
  std::string oneLine;
  while (std::getline(file, oneLine)) directive.insert(oneLine);
}

void SemanticAnalysis::openFile(const std::string &filePath,
                                std::ifstream &file) {
  file.open(filePath);
  if (file.is_open() == false) throw CantOpenFile(filePath);
}

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
    throw InvalidDirective(getThrowMessage());
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
  _config->_httpContext._serverContext.push_back(
      std::make_shared<ServerContext>());
}

void SemanticAnalysis::createLocationContext() noexcept {
  _config->_httpContext._serverContext.back()->_locationContext.push_back(
      std::make_shared<Location>());

  _config->_httpContext._serverContext.back()
      ->_locationContext.back()
      ->_urlValue = _tokenLine[1]->_tokenStr;
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

void SemanticAnalysis::saveDirectiveValue() {
  mainDirectiveSave();
  httpSaveDirective();
  serverSaveDirective();
  locationSaveDirective();
}

void SemanticAnalysis::mainDirectiveSave() {
  if (isDirectiveInLine(_mainValidDirective) == false) return;

  if (isValidMainDirective() == false)
    throw InvalidMainDirective(_tokenLine.front()->_foundLine + ": " +
                               currentLine());
  else if (canDirectiveBeSaved(TypeToken::PID) == true)
    _config->_pidValue = _tokenLine[1]->_tokenStr;
  else if (canDirectiveBeSaved(TypeToken::WORKER_PROCESS) == true)
    _config->_workerProcessesValue = _tokenLine[1]->_tokenStr;
  else if (canDirectiveBeSaved(TypeToken::ERROR_LOG) == true)
    _config->_errorLogValue = _tokenLine[1]->_tokenStr;
  else
    throw DirectiveWasAlreadySet(getThrowMessage());
}

void SemanticAnalysis::httpSaveDirective() {
  if (isDirectiveInLine(_httpValidDirective) == false) return;

  if (isValidHttpDirective() == false)
    throw InvalidHttpDirective(getThrowMessage());
  else if (canDirectiveBeSaved(TypeToken::GEO_IP_COUNTRY) == true)
    _config->_httpContext._geoipCountryValue = _tokenLine[1]->_tokenStr;
  else if (canDirectiveBeSaved(TypeToken::PROXY_CACHE_PATH) == true)
    _config->_httpContext._proxyCachePathValue = _tokenLine[1]->_tokenStr;
  else if (canDirectiveBeSaved(TypeToken::PROXY_CACHE) == true)
    _config->_httpContext._proxyCacheValue = _tokenLine[1]->_tokenStr;
  else if (canDirectiveBeSaved(TypeToken::GZIP) == true)
    _config->_httpContext._gzipValue = _tokenLine[1]->_tokenStr;
  else if (canDirectiveBeSaved(TypeToken::LIMIT_RED_ZONE) == true)
    _config->_httpContext._limitReqZoneValue = _tokenLine[1]->_tokenStr;
  else if (canDirectiveBeSaved(TypeToken::PROXY_CACHE_USE_STALE) == true)
    saveMultipleDirectiveValue(_config->_httpContext._proxyCacheUseStaleValue);
  else if (canDirectiveBeSaved(TypeToken::GZIP_TYPES) == true)
    saveMultipleDirectiveValue(_config->_httpContext._gzipTypesValue);
  else if (_tokenLine.front()->_type == TypeToken::PROXY_SET_HEADER)
    saveMultipleDirectiveValue(_config->_httpContext._proxySetHeaderValue);
  else if (_tokenLine.front()->_type == TypeToken::PROXY_CACHE_VALID)
    saveMultipleDirectiveValue(_config->_httpContext._proxyCacheValidValue);
}

void SemanticAnalysis::serverSaveDirective() {
  if (isDirectiveInLine(_serverValidDirective) == false) return;

  if (serverValidDirective() == false)
    throw InvalidLocationDirective(getThrowMessage());

  std::string value = _tokenLine[1]->_tokenStr;
  if (canClientMaxBodySetBeSet() == true)
    setClientMaxBodySize();
  else if (canDirectiveBeSaved(TypeToken::SSL_CERTIFICATE))
    _config->_httpContext._serverContext.back()->_sslCertificateValue = value;
  else if (canDirectiveBeSaved(TypeToken::SSL_CERTIFICATE_KEY))
    _config->_httpContext._serverContext.back()->_sslCertificateKeyValue =
        value;
  else if (canDirectiveBeSaved(TypeToken::ROOT))
    _config->_httpContext._serverContext.back()->_rootValue = value;
  else if (canDirectiveBeSaved(TypeToken::INDEX))
    saveMultipleDirectiveValue(
        _config->_httpContext._serverContext.back()->_indexValue);
  else if (canDirectiveBeSaved(TypeToken::SERVER_NAME))
    saveMultipleDirectiveValue(
        _config->_httpContext._serverContext.back()->_serverNameValue);
  else if (canDirectiveBeSaved(TypeToken::SERVER_NAME))
    saveMultipleDirectiveValue(
        _config->_httpContext._serverContext.back()->_serverNameValue);
  else if (canDirectiveBeSaved(TypeToken::UPLOAD_DIR))
    _config->_httpContext._serverContext.back()->_uploadDirValue = value;
  else if (_tokenLine.front()->_type == TypeToken::ERROR_PAGE &&
           _tokenLine.size() == 3)
    errorPageSave();
  else if (canDirectiveBeSaved(TypeToken::REQUEST_TIMEOUT))
    saveRequestTimeOut();
  else if (validListen() == true)
    saveListenValue();
  else
    throw InvalidServerDirective(getThrowMessage());
}

void SemanticAnalysis::locationSaveDirective() {
  if (isDirectiveInLine(_locationValidDirective) == false) return;

  if (locationValidDirective() == false)
    throw InvalidLocationDirective(getThrowMessage());
  else if (locationReturned() == true)
    throw DirectiveSetAfterReturnInLocation(getThrowMessage());

  std::string value = _tokenLine[1]->_tokenStr;
  if (canDirectiveBeSaved(TypeToken::PROXY_PASS))
    _config->_httpContext._serverContext.back()
        ->_locationContext.back()
        ->_proxyPassValue = value;
  else if (canDirectiveBeSaved(TypeToken::ALIAS))
    _config->_httpContext._serverContext.back()
        ->_locationContext.back()
        ->_aliasValue = value;
  else if (canDirectiveBeSaved(TypeToken::AUTO_INDEX))
    _config->_httpContext._serverContext.back()
        ->_locationContext.back()
        ->_autoIndexValue = value;
  else if (_tokenLine.front()->_type == TypeToken::TRY_FILES &&
           _state == State::LOCATION_CONTEXT)
    saveMultipleDirectiveValue(_config->_httpContext._serverContext.back()
                                   ->_locationContext.back()
                                   ->_tryFilesValue);
  else if (_tokenLine.front()->_type == TypeToken::ERROR_PAGE &&
           _state == State::LOCATION_CONTEXT)
    saveMultipleDirectiveValue(_config->_httpContext._serverContext.back()
                                   ->_locationContext.back()
                                   ->_errorPageValue);
  else if (_tokenLine.front()->_type == TypeToken::ERROR_PAGE &&
           _state == State::LOCATION_CONTEXT)
    saveMultipleDirectiveValue(_config->_httpContext._serverContext.back()
                                   ->_locationContext.back()
                                   ->_errorPageValue);
  else if (_tokenLine.front()->_type == TypeToken::ACCESS_LOG &&
           _state == State::LOCATION_CONTEXT)
    saveMultipleDirectiveValue(_config->_httpContext._serverContext.back()
                                   ->_locationContext.back()
                                   ->_accessLogValue);
  else if (_tokenLine.front()->_type == TypeToken::INCLUDE &&
           _state == State::LOCATION_CONTEXT)
    saveMultipleDirectiveValue(_config->_httpContext._serverContext.back()
                                   ->_locationContext.back()
                                   ->_includeValue);
  else if (_tokenLine.front()->_type == TypeToken::DENY &&
           _state == State::LOCATION_CONTEXT)
    saveMultipleDirectiveValue(_config->_httpContext._serverContext.back()
                                   ->_locationContext.back()
                                   ->_denyValue);
  else if (canDirectiveBeSaved(TypeToken::ALLOW_METHODS))
    saveMultipleDirectiveValue(_config->_httpContext._serverContext.back()
                                   ->_locationContext.back()
                                   ->_allowMethods);
  else if (canDirectiveBeSaved(TypeToken::RETURN))
    saveReturnValue();
  else if (_tokenLine.front()->_type == TypeToken::CGI &&
           _tokenLine.size() == 3)
    _config->_httpContext._serverContext.back()
        ->_locationContext.back()
        ->_cgi.insert({_tokenLine[1]->_tokenStr, _tokenLine[2]->_tokenStr});
  else
    throw InvalidLocationDirective(getThrowMessage());
}

bool SemanticAnalysis::isDirectiveInLine(
    std::set<std::string> &contextType) noexcept {
  for (auto it = _tokenLine.begin(); it != _tokenLine.end(); it++) {
    if (contextType.find((*it)->_tokenStr) != contextType.end()) return (true);
  }
  return (false);
}

bool SemanticAnalysis::isValidMainDirective() noexcept {
  if (_mainValidDirective.find(_tokenLine.front()->_tokenStr) !=
          _mainValidDirective.end() &&
      _tokenLine.size() == 2 && _tokenLine[1]->_type == TypeToken::VALUE &&
      _state == State::MAIN_CONTEXT)
    return (true);
  return (false);
}

bool SemanticAnalysis::isValidHttpDirective() noexcept {
  if (multipleValueValidDirective(_httpMultipleValueValidDirective) == true &&
      _state == State::HTTPS_CONTEXT)
    return (true);
  else if (isValidSingleValueDirective(_httpSingleValueValidDirective) ==
               true &&
           _state == State::HTTPS_CONTEXT)
    return (true);
  return (false);
}

bool SemanticAnalysis::multipleValueValidDirective(
    const std::set<std::string> &contextType) const noexcept {
  if (contextType.find(_tokenLine.front()->_tokenStr) == contextType.end())
    return (false);
  else if (_tokenLine.size() < 2)
    return (false);
  else if (_tokenLine.back()->_semikolonSet == false)
    return (false);

  for (auto it = (_tokenLine.begin() + 1); it != _tokenLine.end(); it++) {
    if ((*it)->_type != TypeToken::VALUE) return (false);
  }
  return (true);
}

bool SemanticAnalysis::isValidSingleValueDirective(
    const std::set<std::string> &contextType) const noexcept {
  if (contextType.find(_tokenLine.front()->_tokenStr) != contextType.end() &&
      _tokenLine.size() == 2)
    return (true);
  return (false);
}

bool SemanticAnalysis::canDirectiveBeSaved(TypeToken type) const noexcept {
  if (_tokenLine.front()->_type == type && isValueEmpty(type) == true)
    return (true);
  return (false);
}

bool SemanticAnalysis::serverValidDirective() const noexcept {
  if (multipleValueValidDirective(_serverMultiValidDirective) == true &&
      _state == State::SERVER_CONTEXT)
    return (true);
  else if (isValidSingleValueDirective(_serverSingleValDirec) == true &&
           _state == State::SERVER_CONTEXT)
    return (true);
  return (false);
}

bool SemanticAnalysis::locationValidDirective() const noexcept {
  if (multipleValueValidDirective(_multiLocationValDirec) == true &&
      _state == State::LOCATION_CONTEXT)
    return (true);
  else if (isValidSingleValueDirective(_singleLocationValDirec) == true &&
           _state == State::LOCATION_CONTEXT)
    return (true);
  return (false);
}

bool SemanticAnalysis::isValueEmpty(TypeToken token) const noexcept {
  switch (token) {
    break;
    case TypeToken::WORKER_PROCESS:
      if (_config->_workerProcessesValue.empty() == true) return (true);
      break;
    case TypeToken::PID:
      if (_config->_pidValue.empty() == true) return (true);
      break;
    case TypeToken::ERROR_LOG:
      if (_config->_errorLogValue.empty() == true) return (true);
      break;
    case TypeToken::GEO_IP_COUNTRY:
      if (_config->_httpContext._geoipCountryValue.empty() == true)
        return (true);
      break;
    case TypeToken::PROXY_CACHE:
      if (_config->_httpContext._proxyCacheValue.empty() == true) return (true);
      break;
    case TypeToken::PROXY_CACHE_USE_STALE:
      if (_config->_httpContext._proxyCacheUseStaleValue.empty() == true)
        return (true);
      break;
    case TypeToken::GZIP:
      if (_config->_httpContext._gzipValue.empty() == true) return (true);
      break;
    case TypeToken::GZIP_TYPES:
      if (_config->_httpContext._gzipTypesValue.empty() == true) return (true);
      break;
    case TypeToken::LIMIT_RED_ZONE:
      if (_config->_httpContext._limitReqZoneValue.empty() == true)
        return (true);
      break;
    case TypeToken::PROXY_SET_HEADER:
      if (_config->_httpContext._proxySetHeaderValue.empty() == true)
        return (true);
      break;
    case TypeToken::PROXY_CACHE_VALID:
      if (_config->_httpContext._proxyCacheValidValue.empty() == true)
        return (true);
      break;
    case TypeToken::PROXY_CACHE_PATH:
      if (_config->_httpContext._proxyCachePathValue.empty() == true)
        return (true);
      break;
    case TypeToken::SERVER_NAME:
      if (_config->_httpContext._serverContext.back()
              ->_serverNameValue.empty() == true)
        return (true);
      break;
    case TypeToken::SSL_CERTIFICATE:
      if (_config->_httpContext._serverContext.back()
              ->_sslCertificateValue.empty() == true)
        return (true);
      break;
    case TypeToken::SSL_CERTIFICATE_KEY:
      if (_config->_httpContext._serverContext.back()
              ->_sslCertificateKeyValue.empty() == true)
        return (true);
      break;
    case TypeToken::LISTEN:
      if (_config->_httpContext._serverContext.back()->_listenValue.empty() ==
          true)
        return (true);
      break;
    case TypeToken::INDEX:
      if (_config->_httpContext._serverContext.back()->_indexValue.empty() ==
          true)
        return (true);
      break;
    case TypeToken::ROOT:
      if (_config->_httpContext._serverContext.back()->_rootValue.empty() ==
          true)
        return (true);
      break;
    case TypeToken::UPLOAD_DIR:
      if (_config->_httpContext._serverContext.back()
              ->_uploadDirValue.empty() == true)
        return (true);
      break;
    case TypeToken::REQUEST_TIMEOUT:
      if (_config->_httpContext._serverContext.back()->_requestTimeoutValue ==
          60)
        return (true);
      break;
    case TypeToken::PROXY_PASS:
      if (_config->_httpContext._serverContext.back()
              ->_locationContext.back()
              ->_proxyPassValue.empty() == true)
        return (true);
      break;
    case TypeToken::ALIAS:
      if (_config->_httpContext._serverContext.back()
              ->_locationContext.back()
              ->_aliasValue.empty() == true)
        return (true);
      break;
    case TypeToken::TRY_FILES:
      if (_config->_httpContext._serverContext.back()
              ->_locationContext.back()
              ->_tryFilesValue.empty() == true)
        return (true);
      break;
    case TypeToken::ERROR_PAGE:
      if (_config->_httpContext._serverContext.back()
              ->_locationContext.back()
              ->_errorPageValue.empty() == true)
        return (true);
      break;
    case TypeToken::ACCESS_LOG:
      if (_config->_httpContext._serverContext.back()
              ->_locationContext.back()
              ->_accessLogValue.empty() == true)
        return (true);
      break;
    case TypeToken::DENY:
      if (_config->_httpContext._serverContext.back()
              ->_locationContext.back()
              ->_denyValue.empty() == true)
        return (true);
      break;
    case TypeToken::CGI:
      if (_config->_httpContext._serverContext.back()
              ->_locationContext.back()
              ->_cgi.empty() == true)
        return (true);
      break;
    case TypeToken::REWRITE:
      if (_config->_httpContext._serverContext.back()
              ->_locationContext.back()
              ->_rewriteValue.empty() == true)
        return (true);
      break;
    case TypeToken::AUTO_INDEX:
      if (_config->_httpContext._serverContext.back()
              ->_locationContext.back()
              ->_autoIndexValue.empty() == true)
        return (true);
      break;
    case TypeToken::ALLOW_METHODS:
      if (_config->_httpContext._serverContext.back()
              ->_locationContext.back()
              ->_allowMethods.empty() == true)
        return (true);
      break;
    case TypeToken::RETURN:
      if (_config->_httpContext._serverContext.back()
              ->_locationContext.back()
              ->_returnSet == false)
        return (true);
      break;
    default:
      break;
  }
  return (false);
}

bool SemanticAnalysis::validListen() const noexcept {
  if (_tokenLine.size() != 2 && _tokenLine.front()->_type != TypeToken::LISTEN)
    return (false);

  if (_tokenLine[1]->_tokenStr.find_first_not_of("0123456789") !=
          std::string::npos &&
      getIpListenValue(_tokenLine[1]->_tokenStr) == "")
    return (false);

  if (getIpListenValue(_tokenLine[1]->_tokenStr) != "" &&
      getPortListenValue(_tokenLine[1]->_tokenStr) >= 0)
    return (true);
  else if (getPortListenValue(_tokenLine[1]->_tokenStr) >= 0)
    return (true);
  return (false);
}

std::string SemanticAnalysis::getIpListenValue(
    std::string &ipAndPort) const noexcept {
  if (ipAndPort.size() < 8 || ipAndPort.size() > 22)
    return ("");
  else if (ipAndPort.find(":") == std::string::npos)
    return ("");

  std::string ipValue;
  std::istringstream stream(_tokenLine[1]->_tokenStr);
  std::getline(stream, ipValue, ':');
  if (stream.fail() == true || stream.eof() == true || ipValue.empty() == true)
    return ("");

  for (int i = 0; ipValue[i] != '\0'; i++) {
    if (validIpChar(ipValue[i]) == false) return ("");
  }
  return (ipValue);
}

bool SemanticAnalysis::validIpChar(char c) const noexcept {
  if (std::isdigit(c) == false && c != '.') return (false);
  return (true);
}

int SemanticAnalysis::getPortListenValue(
    std::string &ipAndPort) const noexcept {
  if (ipAndPort.size() < 1 || ipAndPort.size() > 22) return (-1);

  int portValue;
  std::stringstream stream(ipAndPort);

  if (ipAndPort.size() > 5) {
    std::string ipValue;
    std::getline(stream, ipValue, ':');
    stream >> portValue;
  } else
    stream >> portValue;

  if (stream.fail() == true || portValue < 0 || portValue > 65535) return (-1);
  return (portValue);
}

void SemanticAnalysis::saveListenValue() const noexcept {
  int port = getPortListenValue(_tokenLine[1]->_tokenStr);
  std::string ip = getIpListenValue(_tokenLine[1]->_tokenStr);

  if (ip.empty() == false)
    _config->_httpContext._serverContext.back()
        ->_portWithAddressListenValue.push_back(std::make_pair(ip, port));
  _config->_httpContext._serverContext.back()->_listenValue.push_back(port);
}

void SemanticAnalysis::listenSetInServerCheck() const {
  for (auto it = _config->_httpContext._serverContext.begin();
       it != _config->_httpContext._serverContext.end(); it++) {
    if ((*it)->_listenValue.size() == 0) throw ListenNotSet();
  }
}

void SemanticAnalysis::setClientMaxBodySize() {
  _config->_httpContext._serverContext.back()->_clientMaxBodySizeValue =
      convertMaxBodySize(_tokenLine[1]->_tokenStr);
  _config->_httpContext._serverContext.back()->_isSetClientMaxBodySizeValue =
      true;
}

size_t SemanticAnalysis::convertMaxBodySize(const std::string &value) const {
  std::string tmpValue = value;
  char c = tmpValue[tmpValue.size() - 1];
  size_t multiplicator = getMaxBodySizeMultiplier(c);
  tmpValue.erase((value.size() - 1), 1);

  if (tmpValue.find_first_not_of("0123456789") != std::string::npos)
    throw MaxBodySizeInvalidSetting(getThrowMessage());

  std::istringstream stream(tmpValue);
  size_t maxBoySizeValue;

  stream >> maxBoySizeValue;

  if (stream.fail() == true)
    throw MaxBodySizeInvalidSetting(getThrowMessage());
  else if (maxBoySizeValue * multiplicator > std::numeric_limits<size_t>::max())
    throw MaxBodySizeInvalidSetting(getThrowMessage());
  return (maxBoySizeValue * multiplicator);
}

size_t SemanticAnalysis::getMaxBodySizeMultiplier(char c) const {
  size_t multiplicator = 0;
  switch (c) {
    break;
    case 'k':
      multiplicator = 1024;
      break;
    case 'M':
      multiplicator = 1024 * 1024;
      break;
    case 'G':
      multiplicator = 1024 * 1024 * 1024;
      break;
    default:
      throw MaxBodySizeInvalidSetting(getThrowMessage());
  }
  return (multiplicator);
}
bool SemanticAnalysis::canClientMaxBodySetBeSet() const noexcept {
  if (isClientMaxBodySizeSet() == false &&
      _tokenLine.front()->_type == TypeToken::CLIENT_MAX_BODY_SIZE &&
      _tokenLine.size() == 2)
    return (true);
  return (false);
}

bool SemanticAnalysis::isClientMaxBodySizeSet() const noexcept {
  if (_config->_httpContext._serverContext.back()
          ->_isSetClientMaxBodySizeValue == true)
    return (true);
  return (false);
}

bool SemanticAnalysis::locationReturned() const noexcept {
  if (_config->_httpContext._serverContext.back()
          ->_locationContext.back()
          ->_returnSet == true)
    return (true);
  return (false);
}

void SemanticAnalysis::saveReturnValue() {
  int statusCode = convertStatusCode(_tokenLine[1]->_tokenStr);

  if (_tokenLine.size() == 2 &&
      isValidThreeHundredStatusRange(statusCode) == false)
    _config->_httpContext._serverContext.back()
        ->_locationContext.back()
        ->_returnValues = std::make_pair(statusCode, "");
  else if (_tokenLine.size() == 3 &&
           isValidThreeHundredStatusRange(statusCode) == true &&
           isValidReturnURL() == true)
    _config->_httpContext._serverContext.back()
        ->_locationContext.back()
        ->_returnValues = std::make_pair(statusCode, _tokenLine[2]->_tokenStr);
  else if (_tokenLine.size() > 3 && isValidReturnURL() == false) {
    std::string returnMessage = getReturnMessage();
    _config->_httpContext._serverContext.back()
        ->_locationContext.back()
        ->_returnValues = std::make_pair(statusCode, returnMessage);
  } else
    throw InvalidLocationDirective(getThrowMessage());

  _config->_httpContext._serverContext.back()
      ->_locationContext.back()
      ->_returnSet = true;
}

int SemanticAnalysis::convertStatusCode(std::string &statusCodeStr) const {
  if (statusCodeStr.find_first_not_of("0123456789") != std::string::npos)
    throw InvalidStatusCode(getThrowMessage());

  int statusCode;
  std::stringstream stream(statusCodeStr);

  stream >> statusCode;

  if (stream.fail() == true || isWithInStatusCodeRange(statusCode) == false)
    throw InvalidStatusCode(getThrowMessage());
  return (statusCode);
}

bool SemanticAnalysis::isWithInStatusCodeRange(int statusCode) const noexcept {
  if ((statusCode >= 100 && statusCode <= 103) ||
      (statusCode >= 200 && statusCode <= 208) || statusCode == 226 ||
      (statusCode >= 300 && statusCode <= 308) ||
      (statusCode >= 400 && statusCode <= 418) ||
      (statusCode >= 421 && statusCode <= 429) || statusCode == 431 ||
      statusCode == 451 || (statusCode >= 500 && statusCode <= 511))
    return (true);
  return (false);
}

bool SemanticAnalysis::isValidThreeHundredStatusRange(
    int statusCode) const noexcept {
  if (statusCode >= 300 && statusCode <= 308) return (true);
  return (false);
}

bool SemanticAnalysis::isValidReturnURL() const noexcept {
  if (_tokenLine.size() == 3 &&
      (_tokenLine[2]->_tokenStr.compare(0, 7, "http://") == 0 ||
       _tokenLine[2]->_tokenStr.compare(0, 8, "https://") == 0))
    return (true);
  return (false);
}

std::string SemanticAnalysis::getReturnMessage() const noexcept {
  if (_tokenLine.size() < 3 && _tokenLine.front()->_type != TypeToken::RETURN)
    return ("");

  std::string returnMesse;
  for (auto it = _tokenLine.begin() + 2; it != _tokenLine.end(); it++) {
    if (it != _tokenLine.end() - 1)
      returnMesse += (*it)->_tokenStr + " ";
    else
      returnMesse += (*it)->_tokenStr;
  }
  return (returnMesse);
}

void SemanticAnalysis::saveRequestTimeOut() {
  if (_tokenLine.size() != 2 ||
      _tokenLine.front()->_type != TypeToken::REQUEST_TIMEOUT ||
      _state != State::SERVER_CONTEXT)
    throw InvalidRequestTimeout(getThrowMessage());

  size_t valueSize = _tokenLine.back()->_tokenStr.size() - 1;
  if (_tokenLine.back()->_tokenStr[valueSize] != 's')
    throw InvalidRequestTimeout(getThrowMessage());
  _tokenLine.back()->_tokenStr.erase(valueSize, 1);

  if (_tokenLine.back()->_tokenStr.find_first_not_of("0123456789") !=
      std::string::npos)
    throw InvalidRequestTimeout(getThrowMessage());

  std::istringstream stream(_tokenLine.back()->_tokenStr);
  int timeoutValue;

  stream >> timeoutValue;

  if (stream.fail() == true || timeoutValue < 1)
    throw InvalidRequestTimeout(getThrowMessage());
  _config->_httpContext._serverContext.back()->_requestTimeoutValue =
      timeoutValue;
}

void SemanticAnalysis::errorPageSave() {
  if (_tokenLine.size() != 3 ||
      _tokenLine.front()->_type != TypeToken::ERROR_PAGE ||
      _state != State::SERVER_CONTEXT)
    throw InvalidErrorPage(getThrowMessage());

  int statusCode = convertStatusCode(_tokenLine[1]->_tokenStr);
  std::string file = _tokenLine.back()->_tokenStr;

  _config->_httpContext._serverContext.back()->_errorPageValue.insert(
      {statusCode, file});
}

std::unique_ptr<ConfigFile> SemanticAnalysis::getConfigFile() {
  return (std::move(_config));
}

std::string SemanticAnalysis::getThrowMessage() noexcept {
  return (_tokenLine.front()->_foundLine + ": " + currentLine());
}

std::string SemanticAnalysis::getThrowMessage() const noexcept {
  return (_tokenLine.front()->_foundLine + ": " + currentLine());
}

std::string SemanticAnalysis::currentLine() noexcept {
  std::string currentStr;
  for (auto it = _tokenLine.begin(); it != _tokenLine.end(); it++) {
    if (it != (_tokenLine.end() - 1))
      currentStr += (*it)->_tokenStr + " ";
    else
      currentStr += (*it)->_tokenStr;
  }
  return (currentStr);
}

std::string SemanticAnalysis::currentLine() const noexcept {
  std::string currentStr;
  for (auto it = _tokenLine.begin(); it != _tokenLine.end(); it++) {
    if (it != (_tokenLine.end() - 1))
      currentStr += (*it)->_tokenStr + " ";
    else
      currentStr += (*it)->_tokenStr;
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
