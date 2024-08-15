#ifndef HTTP_CONTEXT_HPP
#define HTTP_CONTEXT_HPP

#include <iostream>
#include <memory>
#include <vector>

#include "ServerContext.hpp"

class HttpContext {
 public:
  HttpContext();

  void addNewEmptyServer();
  void httpSaveDirectiveValue(const std::string &directive,
                              const std::string &value);

  std::vector<std::unique_ptr<ServerContext>> _serverContext;
  unsigned int _numberOfServers;
  Location _locationContext;

  std::string _geoipCountry;
  std::string _proxyCachePathValue;
  std::string _proxyCacheValue;
  std::string _proxyCacheUseStaleValue;
  std::string _gzipValue;
  std::string _gzipTypesValue; // got multiple values could be saved seperade!
  std::string _limitReqZoneValue;

  std::vector<std::string> _proxySetHeaderValue;
  std::vector<std::string> _proxyCacheValidValue;

  //std::string _indexValue; // Do we need that in the http context?
  //std::string _rootValue; // Do we need that in the http context?
  //std::string _upstreamValue; // Do we inplement that???
  //std::string _accessLogValue; // Do we inplement that here??
  //std::string _errorLogValue; // Do we inplement that here??
};

#endif  // HTTP_CONTEXT_HPP
