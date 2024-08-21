/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpContext.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/12 11:37:24 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/21 15:03:53 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpContext.hpp"
#include <memory>

HttpContext::HttpContext() {
  _geoipCountry.clear();
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
                                         const std::string &value)
{
  if (directive == "geoip_country")
    _geoipCountry = value;
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
                                    const std::string &value)
{
  if (key == "server")
    _serverContext.push_back(std::make_unique<ServerContext>());
  _serverContext.back()->serverSaveDirectiveValue(key, value);
}

