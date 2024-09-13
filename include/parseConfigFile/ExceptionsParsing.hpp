/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExceptionsParsing.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 10:41:40 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/13 10:08:17 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <string>

class ExceptionsParsing : public std::runtime_error {
 public:
  ExceptionsParsing(const std::string &message) : std::runtime_error(message) {}
};

class InvalidFormat : public ExceptionsParsing {
 public:
  InvalidFormat(const std::string &message)
      : ExceptionsParsing("Invalid config file format: " + message) {}
};

class InvalidContext : public ExceptionsParsing {
 public:
  InvalidContext(const std::string &message)
      : ExceptionsParsing("Config file parsing failed: " + message) {}
};

class CantOpenFile : public ExceptionsParsing {
 public:
  CantOpenFile(const std::string &message)
      : ExceptionsParsing("Can't open file: " + message) {}
};

class InvalidDirective : public ExceptionsParsing {
 public:
  InvalidDirective(const std::string &message)
      : ExceptionsParsing("Invalide directive in config file: " + message) {}
};

class InvalidSetting : public ExceptionsParsing {
 public:
  InvalidSetting(const std::string &message)
      : ExceptionsParsing("Invalid settings " + message) {}
};

class NoOpeningBracketFound : public ExceptionsParsing {
 public:
  NoOpeningBracketFound(const std::string &message)
      : ExceptionsParsing("No opening bracket found. Line " + message) {}
};

class MissingSymbol : public ExceptionsParsing {
 public:
  MissingSymbol(const std::string &message)
      : ExceptionsParsing("Missing symbol. Line " + message) {}
};

class EmptyVectorOfTokens : public ExceptionsParsing {
 public:
  EmptyVectorOfTokens() : ExceptionsParsing("Can't get empty tokens") {}
};

class DirectiveValueNotTerminatedWithSemicolon : public ExceptionsParsing {
 public:
  DirectiveValueNotTerminatedWithSemicolon(const std::string &message)
      : ExceptionsParsing(
            "Directive value is not terminated by semicolon. line " + message) {
  }
};

class NoClosingBracketFound : public ExceptionsParsing {
 public:
  NoClosingBracketFound() : ExceptionsParsing("Missing closing bracket") {}
};

class InvalidCharacterFound : public ExceptionsParsing {
 public:
  InvalidCharacterFound(const std::string &message)
      : ExceptionsParsing("Invalid character found. Line " + message) {}
};

class CantIdentifySetting : public ExceptionsParsing {
 public:
  CantIdentifySetting(const std::string &message)
      : ExceptionsParsing("Can't identify setting in config. Line " + message) {
  }
};
