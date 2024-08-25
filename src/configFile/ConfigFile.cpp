/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/10 12:06:57 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/23 13:58:05 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

#include <string.h>
#include <cstring>
#include <fstream>
#include <string>

#include "HttpContext.hpp"
#include "ServerContext.hpp"

ConfigFile::ConfigFile() {
  _workerProcessesValue.clear();
  _pidValue.clear();
  _errorLogValue.clear();
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
  if (_state == SERVER_CONTEXT_IN_HTTP && line.find("server") != line.npos)
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
  std::string key;
  value = line;

  removeWhiteSpacesFront(value);
  getKey(line, key);
  value.erase(0, key.size() + 1);
  value.erase(value.size() - 1, value.size() - 1);
}

void ConfigFile::httpContextSave(const std::string &line) {
  if (_state != HTTPS_CONTEXT && line.find("http") == line.npos) return;
  std::string key;
  std::string value;

  getKey(line, key);
  getValue(line, value);
  _httpContext.httpSaveDirectiveValue(key, value);
}

void ConfigFile::getKey(const std::string &line, std::string &key) {
  if (line.empty() == true || numberOfWordsSeperatedBySpaces(line) < 2) return;
  char delimiter = ' ';
  char *modified_line = strdup(line.c_str());
  key = std::strtok(modified_line, &delimiter);
  free(modified_line);
}

void ConfigFile::serverContextSave(const std::string &line) {
  if (_state != SERVER_CONTEXT_IN_HTTP || line.find("server") == line.npos ||
      numberOfWordsSeperatedBySpaces(line) < 2)
    return;
  std::string key;
  std::string value;
  getKey(line, key);
  getValue(line, value);
  _httpContext.serverSaveContextOrDirective(key, value);
}

void ConfigFile::locationContextSave(const std::string &line) {
  if (_state != LOCATION_CONTEXT_IN_SERVER ||
      numberOfWordsSeperatedBySpaces(line) < 2)
    return;
  std::string key;
  std::string value;
  getKey(line, key);
  getValue(line, value);
  _httpContext._serverContext.back()->locationSaveDirectiveValue(key, value);
}

void ConfigFile::removeWhiteSpacesFront(std::string &str) {
  int whiteSpaces;

  if (str[0] != ' ') return;

  whiteSpaces = 0;
  while (str[whiteSpaces] != '\0' && str[whiteSpaces] == ' ') whiteSpaces++;

  str.erase(0, whiteSpaces);
}
int ConfigFile::numberOfWordsSeperatedBySpaces(const std::string &str) {
  std::stringstream stream(str);
  std::string oneWord;
  unsigned int numberOfWords;

  numberOfWords = 0;
  while (stream >> oneWord) {
    numberOfWords++;
  }
  return (numberOfWords);
}
