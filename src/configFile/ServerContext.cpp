/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 10:17:31 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/26 18:20:59 by fgabler          ###   ########.fr       */
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
                                               const std::string &value) {
  _locationContext.back()->locationSaveDirectiveValue(key, value);
}

void ServerContext::createNewLocation(const std::string &url) {
  _locationContext.push_back(std::make_unique<Location>());
  _locationContext.back()->initializeLocation(url);
}

void ServerContext::printServerContent() const {
  std::cout << "-Server-\n"
            << "ssl certificate: [" << _sslCertificateValue << "]" << std::endl
            << "ssl certificate key: [" << _sslCertificateKeyValue << "]"
            << std::endl
            << "index: [" << _indexValue << "]" << std::endl
            << "root: [" << _rootValue << "]" << std::endl;

  auto listen_it = _listenValue.begin();
  while (listen_it != _listenValue.end()) {
    std::cout << "listen: [" << *listen_it << "]" << std::endl;
    listen_it++;
  }

  auto it_location = _locationContext.begin();

  std::cout << std::endl;
  while (it_location != _locationContext.end()) {
    (*it_location)->printLocation();
    it_location++;
  }
}
