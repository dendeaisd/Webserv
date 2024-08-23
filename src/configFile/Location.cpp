/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 15:39:42 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/23 13:16:20 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include <string.h>

Location::Location()
{
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

void Location::initializeLocation(const std::string &url)
{
  _urlValue = url;
}


void Location::locationSaveDirectiveValue(const std::string &key,
                                  const std::string &value)
{
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

void Location::cgiSetSeparatedValue(const std::string &value)
{
  std::string fileType;
  std::string pathToInterpreter;
  char delimiter = ' ';
  char *tmp_value;

  tmp_value = strdup(value.c_str());
  fileType = std::strtok(tmp_value, &delimiter);
  pathToInterpreter = std::strtok(NULL, &delimiter);
  _cgi.insert({fileType, pathToInterpreter});
  free(tmp_value);
}
