/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExceptionsTokenization.hpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 10:41:40 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/08 08:00:59 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <string>

class ExceptionsToken : public std::runtime_error {
 public:
  ExceptionsToken(const std::string &message) : std::runtime_error(message) {}
};

class invalidFormat : public ExceptionsToken {
 public:
  invalidFormat(const std::string &message)
      : ExceptionsToken("Invalid config file format: " + message) {}
};

class invalidContext : public ExceptionsToken {
 public:
  invalidContext(const std::string &message)
      : ExceptionsToken("Config file parsing failed: " + message) {}
};

class cantOpenFile : public ExceptionsToken {
 public:
  cantOpenFile(const std::string &message)
      : ExceptionsToken("Can't open file: " + message) {}
};

class invalidDirective : public ExceptionsToken {
 public:
  invalidDirective(const std::string &message)
      : ExceptionsToken("Invalide directive in config file: " + message) {}
};

class invalidSetting : public ExceptionsToken {
 public:
  invalidSetting(const std::string &message)
      : ExceptionsToken("Invalid settings " + message) {}
};
