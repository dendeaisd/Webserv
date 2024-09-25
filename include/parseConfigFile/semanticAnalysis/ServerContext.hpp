#ifndef SERVER_CONTEXT_HPP
#define SERVER_CONTEXT_HPP

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Location.hpp"

class ServerContext {
 public:
  ServerContext();

  void printServerContent() const noexcept;

  bool _isSetClientMaxBodySizeValue;
  size_t _clientMaxBodySizeValue;

  int _requestTimeoutValue;

  std::string _sslCertificateValue;
  std::string _sslCertificateKeyValue;
  std::string _rootValue;
  std::string _uploadDirValue;

  std::vector<std::string> _indexValue;
  std::vector<std::string> _serverNameValue;

  std::unordered_map<int, std::string> _errorPageValue;

  /*_listenValue should store only a vector of ints that contains the ports*/
  std::vector<int> _listenValue;
  std::vector<std::pair<std::string, int>> _portWithAddressListenValue;

  std::vector<std::shared_ptr<Location>> _locationContext;

 private:
  template <typename T>
  void printTypeFormat(const std::string &type, const T &value) const noexcept {
    std::cout << type << ": [" << value << "]\n";
  }

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
  void errorPagePrint() const noexcept;
};

#endif
