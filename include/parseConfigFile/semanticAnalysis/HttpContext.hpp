#ifndef HTTP_CONTEXT_HPP
#define HTTP_CONTEXT_HPP

#include <iostream>
#include <memory>
#include <vector>

#include "ServerContext.hpp"

class HttpContext {
 public:
  HttpContext();

  void printHttpContent() const noexcept;

  std::string _geoipCountryValue;
  std::string _proxyCachePathValue;
  std::string _proxyCacheValue;
  std::string _gzipValue;
  std::string _limitReqZoneValue;

  std::vector<std::string> _proxyCacheUseStaleValue;
  std::vector<std::string> _gzipTypesValue;
  std::vector<std::string> _proxySetHeaderValue;
  std::vector<std::string> _proxyCacheValidValue;
  std::vector<std::unique_ptr<ServerContext> > _serverContext;

 private:
  void typeFormatPrint(const std::string &type, const std::string &value) const noexcept;
  void printVectorOfStrings(const std::vector<std::string> &vec,
                            const std::string valueType) const noexcept;
  // std::string _indexValue; // Do we need that in the http context?
  // std::string _rootValue; // Do we need that in the http context?
  // std::string _upstreamValue; // Do we inplement that???
  // std::string _accessLogValue; // Do we inplement that here??
  // std::string _errorLogValue; // Do we inplement that here??
};

#endif  // HTTP_CONTEXT_HPP
