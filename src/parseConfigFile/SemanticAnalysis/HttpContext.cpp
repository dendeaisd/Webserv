/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpContext.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/12 11:37:24 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/20 10:48:51 by fgabler          ###   ########.fr       */
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

void HttpContext::printHttpContent() const noexcept {
  std::cout << "-Http-\n";
  typeFormatPrint("geo ip country", _geoipCountryValue);
  typeFormatPrint("proxy_cache_path", _proxyCachePathValue);
  typeFormatPrint("proxy_cache", _proxyCacheValue);
  typeFormatPrint("gzip", _gzipValue);
  typeFormatPrint("limit_req_zone", _limitReqZoneValue);
  printVectorOfStrings(_gzipTypesValue, "gzip_types");
  printVectorOfStrings(_proxyCacheUseStaleValue, "proxy_cache_use_stale");
  printVectorOfStrings(_proxySetHeaderValue, "proxy_set_header");
  printVectorOfStrings(_proxyCacheValidValue, "proxy_cache_valid");

  std::cout << "\n";
  auto it = _serverContext.begin();

  while (it != _serverContext.end()) {
    (*it)->printServerContent();
    it++;
  }
}

void HttpContext::typeFormatPrint(const std::string &type,
                                  const std::string &value) const noexcept {
  std::cout << type << ": [" << value << "]\n";
}

void HttpContext::printVectorOfStrings(
    const std::vector<std::string> &vec,
    const std::string valueType) const noexcept {
  auto it = vec.begin();
  int i;
  i = 0;
  while (it != vec.end()) {
    std::cout << "[" << i << "] " << valueType << ": [" << *it << "]\n";
    it++;
    i++;
  }
}
