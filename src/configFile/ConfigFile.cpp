/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/10 12:06:57 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/11 19:16:43 by fgabler          ###   ########.fr       */
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
  else if (_state == HTTPS_CONTEXT && _httpBracket.size() == 0)
    _state = MAIN_CONTEXT;
  else if (_state == SERVER_CONTEXT_IN_HTTP && _serverBracket.size() == 0)
    _state = HTTPS_CONTEXT;
  else if (_state == LOCATION_CONTEXT_IN_SERVER && _locationBracket.size() == 0)
    _state = SERVER_CONTEXT_IN_HTTP;
}

void ConfigFile::trackBrackets(const std::string &line) {
  for (auto c : line) {
     if (c == '{')
       pushBracketForCurrentState();
     else if (c == '}')
       popBracketForCurrentState();
  }
}

void ConfigFile::popBracketForCurrentState() {
  switch (_state) {
    case MAIN_CONTEXT:
      break;
    case HTTPS_CONTEXT:
      _httpBracket.pop();
      break;
    case SERVER_CONTEXT_IN_HTTP:
      _serverBracket.pop();
      break;
    case LOCATION_CONTEXT_IN_SERVER:
      _locationBracket.pop();
      break;
  }
}

void ConfigFile::pushBracketForCurrentState() {
  switch (_state) {
    case MAIN_CONTEXT:
      break;
    case HTTPS_CONTEXT:
      _httpBracket.push('{');
      break;
    case SERVER_CONTEXT_IN_HTTP:
      _serverBracket.push('{');
      break;
    case LOCATION_CONTEXT_IN_SERVER:
      _locationBracket.push('{');
      break;
  }
}
