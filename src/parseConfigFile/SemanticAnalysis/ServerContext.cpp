/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 10:17:31 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/17 11:50:26 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerContext.hpp"

#include <cstring>
#include <memory>
#include <sstream>
#include <string>

ServerContext::ServerContext() {
  _listenValue.clear();
  _serverNameValue.clear();
  _sslCertificateValue.clear();
  _sslCertificateKeyValue.clear();
  _indexValue.clear();
  _rootValue.clear();
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

  for (auto it = _portWithAdressListenValue.begin();
       it != _portWithAdressListenValue.end(); it++) {
    std::cout << "listen: adress [" << (*it).first << "] port [" << (*it).second
              << "]\n";
  }

  auto it_location = _locationContext.begin();

  std::cout << std::endl;
  while (it_location != _locationContext.end()) {
    (*it_location)->printLocation();
    it_location++;
  }
}

void ServerContext::addListen(const std::string &value) {
  int port;
  std::istringstream stream(value);

  if (value.find(':') == std::string::npos) {
    stream >> port;
    _listenValue.push_back(port);
  } else if (value.find(':') != std::string::npos) {
    std::string adress;
    std::getline(stream, adress, ':');
    stream >> port;
    _portWithAdressListenValue.push_back(std::make_pair(adress, port));
  }
}
