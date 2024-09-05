/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenization.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/31 16:06:22 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/05 15:28:58 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZATION_HPP
#define TOKENIZATION_HPP

#include <string>
#include <vector>
#include <memory>
#include <set>

#include "NodeToken.hpp"
#include "TypeToken.hpp"
#include "fstream"

class Tokenization {
 public:
  Tokenization(std::ifstream &file);
  Tokenization() = delete;
  Tokenization(const Tokenization &) = delete;
  Tokenization &operator=(const Tokenization &) = delete;
  std::vector<TokenNode> getTokens() const;

 private:
  void loadInvalidContextsAndDirectives();
  void separateTokenStringsFromLine(std::string &line);
  void lineNumberAddToLineOfTokens(size_t currentLineNumber);
  void identifyTokenLineTypes();
  void checkAndSetSemikolonInToken();
  void removeSemikolonFromToken();
  void bracketIdentification();
  void contextIdentification();
  bool isInvalidContext(const std::string &context);
  void addToLineTokensToTokenChain();
  void clearTokenLine();

  std::vector<std::unique_ptr<TokenNode> > _tokensFromLine;
  std::vector<std::unique_ptr<TokenNode> > _chainOfTokens;
  std::set<std::string> _invalidContext;
  std::set<std::string> _invalidDirective;
};

#endif
