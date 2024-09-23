#ifndef SERVER_CONTEXT_HPP
#define SERVER_CONTEXT_HPP

#include <iostream>
#include <memory>
#include <vector>

#include "Location.hpp"

class ServerContext {
 public:
  ServerContext();

  void printServerContent() const noexcept;
  // void addListen(const std::string &value);

  std::string _clientMaxBodySizeValue;
  std::string _sslCertificateValue;
  std::string _sslCertificateKeyValue;
  std::string _rootValue;

  std::vector<std::string> _indexValue;
  std::vector<std::string> _serverNameValue;

  /*_listenValue should store only a vector of ints that contains the ports*/
  std::vector<int> _listenValue;
  std::vector<std::pair<std::string, int>> _portWithAddressListenValue;

  std::vector<std::unique_ptr<Location>> _locationContext;

 private:
  void printTypeFormat(const std::string &type,
                       const std::string &value) const noexcept;
  template <typename T>
  void vectorPrint(const std::vector<T> &vec,
                   const std::string &type) const noexcept {
    int i = 0;
    for (auto it = vec.begin(); it != vec.end(); it++) {
      std::cout << "[" << i << "] " << type << ": [" << (*it) << "]\n";
      i++;
    }
  }
  void vectorPrint(const std::vector<std::pair<std::string, int>> &vec,
                   const std::string &type) const noexcept;
  void listenPrint() const noexcept;
};
#endif
