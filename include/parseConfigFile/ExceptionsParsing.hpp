/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExceptionsParsing.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 10:41:40 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/23 15:33:04 by fgabler          ###   ########.fr       */
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
      : ExceptionsParsing("Invalide directive in config fine line " + message) {
  }
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

class InvalidBracket : public ExceptionsParsing {
 public:
  InvalidBracket(const std::string &message)
      : ExceptionsParsing("Invalid bracket set. Line " + message) {}
};

class InvalidMainDirective : public ExceptionsParsing {
 public:
  InvalidMainDirective(const std::string &message)
      : ExceptionsParsing("Invalid directive in main context. Line" + message) {
  }
};

class InvalidHttpDirective : public ExceptionsParsing {
 public:
  InvalidHttpDirective(const std::string &message)
      : ExceptionsParsing("Invalid directive in http context. Line " +
                          message) {}
};

class DirectiveWasAlreadySet : public ExceptionsParsing {
 public:
  DirectiveWasAlreadySet(const std::string &message)
      : ExceptionsParsing("Directive has been set already. Line " + message) {}
};

class DirectiveSetAtWrongPosition : public ExceptionsParsing {
 public:
  DirectiveSetAtWrongPosition(const std::string &message)
      : ExceptionsParsing("Directive is set at wrong Position. Line " +
                          message) {}
};

class InvalidLocationDirective : public ExceptionsParsing {
 public:
  InvalidLocationDirective(const std::string &message)
      : ExceptionsParsing("Invalid directive in location context. Line " +
                          message) {}
};

class InvalidServerDirective : public ExceptionsParsing {
 public:
  InvalidServerDirective(const std::string &message)
      : ExceptionsParsing("Invalid directive in server context. Line " +
                          message) {}
};

class ListenNotSet : public ExceptionsParsing {
  public:
    ListenNotSet()
      : ExceptionsParsing("Listen is not set in server.") {}
};
