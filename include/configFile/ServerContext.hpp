#ifndef SERVER_CONTEXT_HPP
#define SERVER_CONTEXT_HPP

#include <iostream>
#include <memory>
#include <vector>

#include "Location.hpp"

class ServerContext {
 public:
  ServerContext();

  void serverSaveDirectiveValue(const std::string &key,
                                const std::string &value);
  void locationSaveDirectiveValue(const std::string &key,
                                  const std::string &value);
  void printServerContent() const;

  std::string _serverNameValue;  // A separation of the values can be
                                 // implemented if wanted!

  std::string _sslCertificateValue;
  std::string _sslCertificateKeyValue;
  std::string _indexValue;
  std::string _rootValue;

  std::vector<std::string> _listenValue;
  std::vector<std::unique_ptr<Location> > _locationContext;
};

#endif
