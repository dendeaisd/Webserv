/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 10:17:31 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/22 17:06:40 by fgabler          ###   ########.fr       */
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

void ServerContext::printServerContent() const noexcept {
  std::cout << "-Server-\n";
  printTypeFormat("client_max_body_size", _clientMaxBodySizeValue);
  printTypeFormat("ssl_certificate", _sslCertificateValue);
  printTypeFormat("ssl_certificate_key", _sslCertificateKeyValue);
  printTypeFormat("root", _rootValue);
  vectorPrint(_indexValue, "index");
  vectorPrint(_serverNameValue, "server_name");
  vectorPrint(_listenValue, "listen");
  vectorPrint(_portWithAdressListenValue, "listen");

  auto it_location = _locationContext.begin();

  std::cout << std::endl;
  while (it_location != _locationContext.end()) {
    (*it_location)->printLocation();
    it_location++;
  }
}

void ServerContext::printTypeFormat(const std::string &type,
                                    const std::string &value) const noexcept {
  std::cout << type << ": [" << value << "]\n";
}

void ServerContext::vectorPrint(
    const std::vector<std::pair<std::string, int>> &vec,
    const std::string &type) const noexcept {
  int i = 0;
  for (auto it = vec.begin(); it != vec.end(); it++) {
    std::cout << "[" << i << "] " << type << ": [" << (*it).first << "] ["
              << (*it).second << "]\n";
    i++;
  }
};

/*
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
*/
