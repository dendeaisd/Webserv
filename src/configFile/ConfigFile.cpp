/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/10 12:06:57 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/26 20:58:39 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

#include <string.h>

#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#include "HttpContext.hpp"
#include "ServerContext.hpp"

ConfigFile::ConfigFile() {
  _workerProcessesValue.clear();
  _pidValue.clear();
  _errorLogValue.clear();
}

void ConfigFile::printConfigFileContent() {
  std::cout << "-Main-\n"
            << "worker process: [" << _workerProcessesValue << "]"<< std::endl
            << "pid: [" << _pidValue << "]" << std::endl
            << "error log: [" << _errorLogValue << "]\n\n";
  _httpContext.printHttpContent();
}

void ConfigFile::storeValidConfiguration(const std::string &fileName) {
  std::fstream file;
  std::string line;

  _state = MAIN_CONTEXT;
  file.open(fileName.c_str());
  while (std::getline(file, line)) {
    setCurrentState(line);
    trackBrackets(line);
    possibleNewServerContextSetup(line);
    mainContextSaveDirective(line);
    httpContextSave(line);
    serverContextSave(line);
    possibleNewLocationSetup(line);
    locationContextSave(line);
  }
}

void ConfigFile::setCurrentState(const std::string &line) {
  if (_state == MAIN_CONTEXT && line.find("http") != line.npos)
    _state = HTTPS_CONTEXT;
  else if (_state == HTTPS_CONTEXT && line.find("server") != line.npos)
    _state = SERVER_CONTEXT_IN_HTTP;
  else if (_state == SERVER_CONTEXT_IN_HTTP &&
           line.find("location") != line.npos)
    _state = LOCATION_CONTEXT_IN_SERVER;
  else if (_state == HTTPS_CONTEXT && _bracketStatus[HTTP_BRACKET].size() == 0)
    _state = MAIN_CONTEXT;
  else if (_state == SERVER_CONTEXT_IN_HTTP &&
           _bracketStatus[SERVER_BRACKET].size() == 0)
    _state = HTTPS_CONTEXT;
  else if (_state == LOCATION_CONTEXT_IN_SERVER &&
           _bracketStatus[LOCATION_BRACKET].size() == 0)
    _state = SERVER_CONTEXT_IN_HTTP;
}

void ConfigFile::possibleNewServerContextSetup(const std::string &line) {
  if (isStoringState(line, SERVER_CONTEXT_IN_HTTP) == true)
    _httpContext.addNewEmptyServer();
}

void ConfigFile::trackBrackets(const std::string &line) {
  EBracketStatus status;

  transferStateToBracketStatus(status);
  for (auto c : line) {
    if (c == '#')
      return;
    else if (c == '{')
      _bracketStatus[status].push('{');
    else if (c == '}')
      _bracketStatus[status].pop();
  }
}

void ConfigFile::transferStateToBracketStatus(EBracketStatus &status) {
  switch (_state) {
    case MAIN_CONTEXT:
      status = MAIN_BRACKET;
      break;
    case HTTPS_CONTEXT:
      status = HTTP_BRACKET;
      break;
    case SERVER_CONTEXT_IN_HTTP:
      status = SERVER_BRACKET;
      break;
    case LOCATION_CONTEXT_IN_SERVER:
      status = LOCATION_BRACKET;
      break;
  }
}

void ConfigFile::mainContextSaveDirective(const std::string &line) {
  if (_state != MAIN_CONTEXT) return;
  if (line.find("worker_processes") != line.npos)
    saveDirective(line, _workerProcessesValue);
  else if (line.find("pid") != line.npos)
    saveDirective(line, _pidValue);
  else if (line.find("error_log") != line.npos)
    saveDirective(line, _errorLogValue);
}

void ConfigFile::saveDirective(const std::string &line,
                               std::string &directive) {
  if (line.length() <= 0) return;
  std::string value;
  getValue(line, value);

  directive = value;
}

void ConfigFile::getValue(const std::string &line, std::string &value) {
  if (line.empty() == true || line.length() == 1) return;
  std::istringstream stream(line);
  std::string oneValue;

  stream >> oneValue;
  stream >> oneValue;
  value = value + oneValue;
  while (stream >> oneValue)
    value = value + " " + oneValue;
  if (value[value.size() - 1] == ';')
    value.erase(value.size() - 1);
  //I don't like that way, will change it later, but it works for now
}

void ConfigFile::httpContextSave(const std::string &line) {
  if (_state != HTTPS_CONTEXT || isStoringState(line, HTTPS_CONTEXT) == true)
    return;
  std::string key;
  std::string value;

  getKey(line, key);
  getValue(line, value);
  _httpContext.httpSaveDirectiveValue(key, value);
}

void ConfigFile::getKey(const std::string &line, std::string &key) {
  if (line.empty() == true || numberOfWordsSeparatedBySpaces(line) < 2) return;
  std::istringstream stream(line);

  stream >> key;
}

void ConfigFile::serverContextSave(const std::string &line) {
  if (_state != SERVER_CONTEXT_IN_HTTP ||
      isStoringState(line, SERVER_CONTEXT_IN_HTTP) == true ||
      numberOfWordsSeparatedBySpaces(line) < 2)
    return;
  std::string key;
  std::string value;
  getKey(line, key);
  getValue(line, value);
  _httpContext.serverSaveContextOrDirective(key, value);
}

void ConfigFile::locationContextSave(const std::string &line) {
  if (_state != LOCATION_CONTEXT_IN_SERVER ||
      numberOfWordsSeparatedBySpaces(line) < 2
      || isStoringState(line, LOCATION_CONTEXT_IN_SERVER) == true)
    return;
  std::string key;
  std::string value;
  getKey(line, key);
  getValue(line, value);
  _httpContext._serverContext.back()->locationSaveDirectiveValue(key, value);
}

void ConfigFile::removeWhiteSpacesFront(std::string &str) {
  if (str[0] != ' ') return;
  int whiteSpaces;

  whiteSpaces = 0;
  while (str[whiteSpaces] != '\0' && str[whiteSpaces] == ' ') whiteSpaces++;

  str.erase(0, whiteSpaces);
}

int ConfigFile::numberOfWordsSeparatedBySpaces(const std::string &str) {
  std::stringstream stream(str);
  std::string oneWord;
  unsigned int numberOfWords;

  numberOfWords = 0;
  while (stream >> oneWord) {
    numberOfWords++;
  }
  return (numberOfWords);
}

bool ConfigFile::isStoringState(const std::string &line, EStoringStates state) {
  std::istringstream stream(line);
  std::string onState;
  std::string stateAsStr;

  getStateAsString(stateAsStr, state);
  while (stream >> onState) {
    if (onState == stateAsStr) return (true);
  }
  return (false);
}

void ConfigFile::getStateAsString(std::string &stateStr,
                                  EStoringStates &state) {
  switch (state) {
    case HTTPS_CONTEXT:
      stateStr = "http";
      break;
    case SERVER_CONTEXT_IN_HTTP:
      stateStr = "server";
      break;
    case LOCATION_CONTEXT_IN_SERVER:
      stateStr = "location";
      break;
    default:
      stateStr = "";
  }
}

void ConfigFile::possibleNewLocationSetup(std::string &line) {
  if (isStoringState(line, LOCATION_CONTEXT_IN_SERVER) == false)
    return;
  std::istringstream stream(line);
  std::string url;

  stream >> url;
  stream >> url;
  _httpContext._serverContext.back()->createNewLocation(url);
}