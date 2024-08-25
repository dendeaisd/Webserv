/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpContext.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/12 11:37:24 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/25 21:36:18 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpContext.hpp"

#include <memory>

HttpContext::HttpContext() {
  _geoipCountryValue.clear();
  _proxyCachePathValue.clear();
  _proxyCacheValue.clear();
  _proxyCacheUseStaleValue.clear();
  _gzipValue.clear();
  _gzipTypesValue.clear();
  _limitReqZoneValue.clear();
}

void HttpContext::addNewEmptyServer() {
  _serverContext.push_back(std::make_unique<ServerContext>());
}

void HttpContext::httpSaveDirectiveValue(const std::string &directive,
                                         const std::string &value) {
  if (directive == "geoip_country")
    _geoipCountryValue = value;
  else if (directive == "proxy_cache_path")
    _proxyCachePathValue = value;
  else if (directive == "proxy_cache")
    _proxyCacheValue = value;
  else if (directive == "proxy_cache_use_stale")
    _proxyCacheUseStaleValue = value;
  else if (directive == "gzip")
    _gzipValue = value;
  else if (directive == "gzip_types")
    _gzipTypesValue = value;
  else if (directive == "limit_req_zone")
    _limitReqZoneValue = value;
  else if (directive == "proxy_set_header")
    _proxySetHeaderValue.push_back(value);
  else if (directive == "proxy_cache_valid")
    _proxyCacheValidValue.push_back(value);
}

void HttpContext::serverSaveContextOrDirective(const std::string &key,
                                               const std::string &value) {
  if (key == "server")
    _serverContext.push_back(std::make_unique<ServerContext>());
  _serverContext.back()->serverSaveDirectiveValue(key, value);
}

void HttpContext::printHttpContent() {
  std::cout << "geo ip country: " << _geoipCountryValue << std::endl
            << "proxy chache path: " << _proxyCachePathValue << std::endl
            << "proxy chache: " << _proxyCacheValue << std::endl
            << "proxy chache use stale: " << _proxyCacheUseStaleValue
            << std::endl
            << "gyip " << _gzipValue << std::endl
            << "gzip types: " << _gzipTypesValue << std::endl
            << "limit req zone: " << _limitReqZoneValue << std::endl;
  printVectorOfStrings(_proxySetHeaderValue, "proxy set header");
  printVectorOfStrings(_proxyCacheValidValue, "proxy chache valid");

  auto it = _serverContext.begin();

  while (it != _serverContext.end()) {
    (*it)->printServerContent();
    it++;
  }
}

void HttpContext::printVectorOfStrings(const std::vector<std::string> &vec,
                                       const std::string valueType) const {
  auto it = vec.begin();
  int i;

  i = 0;
  while (it != vec.end()) {
    std::cout << "[" << i << "] " << valueType << ": " << *it << std::endl;
    it++;
  }
}
