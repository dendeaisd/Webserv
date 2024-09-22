/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ramymoussa <ramymoussa@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 15:39:42 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/22 16:16:58 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

#include <string.h>

#include <sstream>

Location::Location() {
  //  _indexValue.clear();
  //  _rootValue.clear();
  _returnSet = false;
  _urlValue.clear();
  _proxyPassValue.clear();
  _aliasValue.clear();
  _tryFilesValue.clear();
  _errorPageValue.clear();
  _accessLogValue.clear();
  _denyValue.clear();
  _statusCodeAndUrlReturnValue.first = 0;
  _statusCodeAndUrlReturnValue.second.clear();
}

void Location::printLocation() {
  std::cout << "-LOCATION-\n";

  printTypeFormat("url", _urlValue);
  printTypeFormat("proxy_pass", _proxyPassValue);
  printTypeFormat("alias", _aliasValue);
  printTypeFormat("auto_index", _autoIndexValue);
  printVecOfStrings(_denyValue, "deny");
  printVecOfStrings(_includeValue, "include");
  printVecOfStrings(_rewriteValue, "rewrite");
  printVecOfStrings(_tryFilesValue, "try_files");
  printVecOfStrings(_errorPageValue, "error_page");
  printVecOfStrings(_accessLogValue, "access_log");

  std::cout << std::endl;
}

void Location::printTypeFormat(const std::string &type,
                     const std::string &value) const noexcept {
  std::cout << type << ": [" << value << "]\n";
}

void Location::printVecOfStrings(const std::vector<std::string> &vec,
                       const std::string &type) const noexcept {
  int i = 0;
  for (auto it = vec.begin(); it != vec.end(); it++, i++) {
    std::cout << "[" << i << "] " << type << ": " << (*it) << std::endl;
  }
}


  /*
     void Location::cgiSetSeparatedValue(const std::string &value) {
     std::string fileType;
     std::string pathToInterpreter;
     std::istringstream stream(value);

     stream >> fileType;
     stream >> pathToInterpreter;
     _cgi.insert({fileType, pathToInterpreter});
     }
     */

  /*
  void Location::addReturn(const std::string &value) {
    std::istringstream stream(value);

    int statusCode;
    std::string url;

    stream >> statusCode;
    stream >> url;

    if (url.empty() == true) {
      _returnStatusCodeValue = statusCode;
    } else {
      _statusCodeAndUrlReturnValue.first = statusCode;
      _statusCodeAndUrlReturnValue.second = url;
    }
  }
  */
