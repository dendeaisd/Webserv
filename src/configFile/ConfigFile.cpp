/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/10 12:06:57 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/14 12:16:16 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

#include <fstream>
#include <string>

void ConfigFile::storeConfiguration(const std::string &fileName) {
  std::fstream file;
  std::string line;

  _state = MAIN_CONTEXT;
  file.open(fileName.c_str());
  while (std::getline(file, line)) {
    setCurrentState(line);
    possibleNewServerContextSetup(line);
    trackBrackets(line);
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

void ConfigFile::possibleNewServerContextSetup(const std::string &line)
{
  if (_state == SERVER_CONTEXT_IN_HTTP && line.find("server") != line.npos)
    _httpContext.addNewEmptyServer();
}

void ConfigFile::trackBrackets(const std::string &line) {
  EBracketStatus status;

  transferStateToBracketStatus(status);
  for (auto c : line) {
    if (c == '{')
      _bracketStatus[status].push('{');
    else if (c == '}')
      _bracketStatus[status].pop();
  }
}

void ConfigFile::transfareStateToBracketStatus(EBracketStatus &status) {
  switch (_state) {
    case MAIN_CONTEXT:
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

void ConfigFile::mainContextSaveDirective(const std::string &line)
{
  if (_state != MAIN_CONTEXT)
    return ;
  if (line.find("worker_processes") != line.npos)
    saveDirective(line, _workerProcessesValue);
  else if (line.find("pid") != line.npos)
    saveDirective(line, _pidValue);
  else if (line.find("error_log") != line.npos)
    saveDirective(line, _errorLogValue);
}

void ConfigFile::saveDirective(const std::string &line, std::string &directive)
{
  std::string value;
  getValue(line, value);

  directive = value;
}

void ConfigFile::getValue(const std::string &line, std::string &value)
{
  int endOfKey;
  while (line[endOfKey] != ' ' && line[endOfKey] != '\0')
  {
    endOfKey++;
  }
  value = line;
  value.erase(0, endOfKey);
  value.erase(value.size() - 1, value.size() - 1);
}


