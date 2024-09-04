/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExceptionsTokenization.hpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 10:41:40 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/04 13:49:38 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <string>

class ExceptionsToken : public std::runtime_error {
 public:
  ExceptionsToken(const std::string &message) : std::runtime_error(message) {}
};

class invalidContext : public ExceptionsToken {
  public:
    invalidContext(const std::string &message)
      : ExceptionsToken("Config file parsing failed: " + message) {}
};

class invalidFormat : public ExceptionsToken {
  public:
    invalidFormat(const std::string &message)
      : ExceptionsToken("Invalid config file format: " + message) {}
};

