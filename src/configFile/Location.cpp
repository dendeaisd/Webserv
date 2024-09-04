/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ramymoussa <ramymoussa@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 15:39:42 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/04 19:37:28 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

#include <string.h>
#include <sstream>

Location::Location() {
  _urlValue.clear();
  _rootValue.clear();
  _proxyPassValue.clear();
  _aliasValue.clear();
  _tryFilesValue.clear();
  _indexValue.clear();
  _errorPageValue.clear();
  _accessLogValue.clear();
  _denyValue.clear();
}

void Location::initializeLocation(const std::string &url) { _urlValue = url; }

void Location::locationSaveDirectiveValue(const std::string &key,
                                          const std::string &value) {
  if (key == "root")
    _rootValue = value;
  else if (key == "proxy_pass")
    _proxyPassValue = value;
  else if (key == "alias")
    _aliasValue = value;
  else if (key == "try_files")
    _tryFilesValue = value;
  else if (key == "index")
    _indexValue = value;
  else if (key == "error_page")
    _errorPageValue = value;
  else if (key == "access_log")
    _accessLogValue = value;
  else if (key == "deny")
    _denyValue = value;
  else if (key == "include")
    _includeValue = value;
  else if (key == "cgi")
    cgiSetSeparatedValue(value);
}

void Location::cgiSetSeparatedValue(const std::string &value) {
  std::string fileType;
  std::string pathToInterpreter;
  std::istringstream stream(value);

  stream >> fileType;
  stream >> pathToInterpreter;
  _cgi.insert({fileType, pathToInterpreter});
}

void Location::printLocation() {
  std::cout << "-LOCATION-\n";
  std::cout << "url: [" << _urlValue << "]" << std::endl
            << "root: [" << _rootValue << "]" << std::endl
            << "include: [" << _includeValue << "]" << std::endl
            << "proxy pass: [" << _proxyPassValue << "]" << std::endl
            << "alias: [" << _aliasValue << "]" << std::endl
            << "try files: [" << _tryFilesValue << "]" << std::endl
            << "index: [" << _indexValue << "]" << std::endl
            << "error page: [" << _errorPageValue << "]" << std::endl
            << "access log: [" << _accessLogValue << "]" << std::endl
            << "deny: [" << _denyValue << "]\n";

  auto it_rewrite = _rewriteValue.begin();

  while (it_rewrite != _rewriteValue.end()) {
    std::cout << "rewrite: [" << *it_rewrite << "]" << std::endl;
    it_rewrite++;
  }

  auto it_cgi = _cgi.begin();

  while (it_cgi != _cgi.end()) {
    std::cout << "GCI: "
              << "file type: [" << (*it_cgi).first << "] path to interpreter: ["
              << (*it_cgi).second << "]\n";
    it_cgi++;
  }
  std::cout << std::endl;
}
