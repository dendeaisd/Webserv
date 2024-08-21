/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 10:17:31 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/21 17:02:00 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerContext.hpp"
#include <cstring>
#include <memory>

ServerContext::ServerContext() {
  _listenValue.clear();
  _serverNameValue.clear();
  _sslCertificateValue.clear();
  _sslCertificateKeyValue.clear();
  _indexValue.clear();
  _rootValue.clear();
}

void ServerContext::serverSaveDirectiveValue(const std::string &key,
                                             const std::string &value) {
  if (key == "server_name")
    _serverNameValue = value;
  else if (key == "ssl_certificate_key")
    _sslCertificateKeyValue = value;
  else if (key == "ssl_certificate_key")
    _sslCertificateKeyValue = value;
  else if (key == "index")
    _indexValue = value;
  else if (key == "root")
    _rootValue = value;
  else if (key == "listen")
    _listenValue.push_back(value);
}

void ServerContext::locationSaveDirectiveValue(const std::string &key,
                                                    const std::string &value)
{
  if (key == "location")
  {
    _locationContext.push_back(std::make_unique<Location>());
    _locationContext.back()->initializeLocation(value);
  }
  else
    _locationContext.back()->locationSaveDirectiveValue(key, value);
}
