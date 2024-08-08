#ifndef SERVER_CONTEXT_HPP
#define SERVER_CONTEXT_HPP

#include <iostream>
#include "Location.hpp"

class ServerContext {
 public:
  ServerContext();

  std::string listenValue_;
  std::string serverNameValue_;
  std::string sslCertificateValue_;
  std::string sslCertificateKeyValue_;
  std::string indexValue_;
  std::string rootValue_;

  Location locationContext_;
};

#endif
