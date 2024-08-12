#ifndef SERVER_CONTEXT_HPP
#define SERVER_CONTEXT_HPP

#include <iostream>
#include "Location.hpp"

class ServerContext {
 public:
  ServerContext();

  std::string _listenValue;
  std::string _serverNameValue;
  std::string _sslCertificateValue;
  std::string _sslCertificateKeyValue;
  std::string _indexValue;
  std::string _rootValue;

  Location _locationContext;

};

#endif
