#ifndef SERVER_CONTEXT_HPP
#define SERVER_CONTEXT_HPP

#include <iostream>
#include <memory>
#include <vector>

#include "Location.hpp"

class ServerContext {
 public:
  ServerContext();

  void printServerContent() const;
  void addListen(const std::string &value);

  std::string _clientMaxBodySizeValue;
  std::string _sslCertificateValue;
  std::string _sslCertificateKeyValue;
  std::string _rootValue;

  std::vector<std::string> _indexValue;
  std::vector<std::string> _serverNameValue;

  /*_listenValue should store only a vector of ints that contains the ports*/
  std::vector<int> _listenValue;
  std::vector<std::pair<std::string, int> > _portWithAdressListenValue;

  std::vector<std::unique_ptr<Location> > _locationContext;
};

#endif
