/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpContext.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/12 11:37:24 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/15 22:25:01 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpContext.hpp"

HttpContext::HttpContext() : _numberOfServers(0) {}

HttpContext::addNewEmptyServer() {
  _serverContext.push_back(std::make_unique<ServerContext>());
  _numberOfServers++;
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
