/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 10:17:31 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/24 19:29:45 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerContext.hpp"

#include <cstring>
#include <limits>
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
  _clientMaxBodySizeValue = SIZE_MAX;
  _requestTimeoutValue = 60;
  _isSetClientMaxBodySizeValue = false;
}

void ServerContext::printServerContent() const noexcept {
  std::cout << "-Server-\n";
  std::cout << "client_max_body_size: [" << _clientMaxBodySizeValue << "]\n";
  printTypeFormat("ssl_certificate", _sslCertificateValue);
  printTypeFormat("ssl_certificate_key", _sslCertificateKeyValue);
  printTypeFormat("root", _rootValue);
  std::cout << "request_timeout [" << _requestTimeoutValue << "]\n";
  printTypeFormat("upload_dir", _uploadDirValue);
  vectorPrint(_indexValue, "index");
  vectorPrint(_serverNameValue, "server_name");
  listenPrint();

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

void ServerContext::listenPrint() const noexcept {
  int i = 0;
  for (auto it = _listenValue.begin(); it != _listenValue.end(); it++) {
    std::cout << "[" << i << "] Listen Value: [" << *it << "]\n";
    i++;
  }
  i = 0;
  for (auto it = _portWithAddressListenValue.begin();
       it != _portWithAddressListenValue.end(); it++) {
    std::cout << "[" << i << "] Listen IP and Port: [" << (*it).first << "] ["
              << (*it).second << "]\n";
  }
}
