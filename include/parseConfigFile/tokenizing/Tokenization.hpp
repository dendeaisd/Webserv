/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenization.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/31 16:06:22 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/31 17:16:48 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZATION_HPP
#define TOKENIZATION_HPP

#include <string>
#include <vector>

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
  std::string getNextLine(std::ifstream &file);
  void seperateTokenStringsFromLine(
      std::string &line, std::vector<std::string> &seperatedTokensStr);
  std::string getNextTokenString(std::string &line);
  void insertIntoTokenNode(std::vector<std::string> &seperatedTokenStr);
  void identifyTokenType();
  void lineNumberAddToTokens();
  void addToLineTokensToTokenChain();
  void clearTokenFromLine();

  std::vector<TokenNode> _tokensFromLine;
  std::vector<TokenNode> _chainOfTokens;
};

#endif
