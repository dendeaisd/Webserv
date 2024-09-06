/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TypeToken.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:29:53 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/06 13:20:13 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "TypeToken.hpp"

std::ostream &operator << (std::ostream &outStream, const TypeToken &type) {
    if (type == TypeToken::WORKER_PROCESS)
      outStream << "WORKER_PROCESS";
    else if(type == TypeToken::PID)
      outStream << "PID";
    else if(type == TypeToken::ERROR_LOG)
      outStream << "ERROR_LOG";
    else if(type == TypeToken::GEO_IP_COUNTRY)
      outStream << "GEO_IP_COUNTRY";
    else if(type == TypeToken::PROXY_CACHE)
      outStream << "PROXY_CACHE";
    else if(type == TypeToken::PROXY_CACHE_USE_STALE)
      outStream << "PROXY_CACHE_USE_STALE";
    else if(type == TypeToken::GZIP)
      outStream << "GZIP";
    else if(type == TypeToken::GZIP_TYPES)
      outStream << "GZIP_TYPES";
    else if(type == TypeToken::LIMIT_RED_ZONE)
      outStream << "LIMIT_RED_ZONE";
    else if(type == TypeToken::PROXY_SET_HEADER)
      outStream << "PROXY_SET_HEADER";
    else if(type == TypeToken::PROXY_CACHE_VALID)
      outStream << "PROXY_CACHE_VALID";
    else if(type == TypeToken::SERVER_NAME)
      outStream << "SERVER_NAME";
    else if(type == TypeToken::SSL_CERTIFICATE)
      outStream << "PROXY_CACHE";
    else if(type == TypeToken::SSL_CERTIFICATE_KEY)
      outStream << "SSL_CERTIFICATE_KEY";
    else if(type == TypeToken::INDEX)
      outStream << "INDEX";
    else if(type == TypeToken::ROOT)
      outStream << "ROOT";
    else if(type == TypeToken::LISTEN)
      outStream << "LISTEN";
    else if(type == TypeToken::URL_LOCATION)
      outStream << "URL_LOCATION";
    else if(type == TypeToken::INCLUDE)
      outStream << "INCLUDE";
    else if(type == TypeToken::PROXY_PASS)
      outStream << "PROXY_PASS";
    else if(type == TypeToken::ALIAS)
      outStream << "ALIAS";
    else if(type == TypeToken::TRY_FILES)
      outStream << "TRY_FILES";
    else if(type == TypeToken::ERROR_PAGE)
      outStream << "ERROR_PAGE";
    else if(type == TypeToken::ACCESS_LOG)
      outStream << "ACCESS_LOG";
    else if(type == TypeToken::DENY)
      outStream << "DENY";
    else if(type == TypeToken::CGI)
      outStream << "CGI";
    else if(type == TypeToken::REWRITE)
      outStream << "REWRITE";
    else if(type == TypeToken::SERVER)
      outStream << "SERVER";
    else if(type == TypeToken::HTTP)
      outStream << "HTTP";
    else if(type == TypeToken::LOCATION)
      outStream << "LOCATION";
    else if(type == TypeToken::OPEN_BRACKET)
      outStream << "OPEN_BRACKET";
    else if(type == TypeToken::CLOSING_BRACKET)
      outStream << "CLOSING_BRACKET";
    else if(type == TypeToken::DEFAULT)
      outStream << "DEFAULT";
    else if (type == TypeToken::PROXY_CACHE_PATH)
      outStream << "PROXY_CACHE_PATH";
    else if (type == TypeToken::CLIENT_MAX_BODY_SIZE)
      outStream << "CLIENT_MAX_BODY_SIZE";
    else if (type == TypeToken::AUTO_INDEX)
      outStream << "AUTO_INDEX";
    else if (type == TypeToken::VALUE)
      outStream << "VALUE";
  return (outStream);
}

