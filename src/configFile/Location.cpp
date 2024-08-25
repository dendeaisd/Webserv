/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 15:39:42 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/25 21:37:29 by fgabler          ###   ########.fr       */
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

void Location::printLocation()
{
  std::cout << "\n-LOCATION-\n\n";
  std::cout << "url: " << _urlValue << std::endl
            << "root: " <<  _rootValue << std::endl
            << "include: " << _includeValue << std::endl
            << "proxy pass: " << _proxyPassValue << std::endl
            << "alias: " << _aliasValue << std::endl
            << _tryFilesValue << std::endl
            << _indexValue << std::endl
            << _errorPageValue << std::endl
            << _accessLogValue << std::endl
            << _denyValue << std::endl;

  auto it_rewrite = _rewriteValue.begin();

  while (it_rewrite != _rewriteValue.end())
  {
    std::cout << "rewrite: " << *it_rewrite << std::endl;
    it_rewrite++;
  }

  auto it_cgi = _cgi.begin();

  while (it_cgi != _cgi.end())
  {
    std::cout << "file type: " << (*it_cgi).first
              << "path to interpreter: " << (*it_cgi).second
              << std::endl;
  }
}

