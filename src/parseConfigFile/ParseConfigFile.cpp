/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfigFile.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 14:44:48 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/22 16:34:43 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseConfigFile.hpp"

#include <fstream>
#include <memory>

#include "ExceptionsParsing.hpp"
#include "SemanticAnalysis.hpp"
#include "Tokenization.hpp"

ParseConfigFile::ParseConfigFile(const std::string &filePath) {
  std::ifstream configFile;

  configFile = getValidFilePath(filePath);
  Tokenization tokenizer(configFile);
  auto setOfTokens = tokenizer.getTokens();
  SemanticAnalysis semanticAnalysis(setOfTokens);
  _config = semanticAnalysis.getConfigFile();
}

std::ifstream ParseConfigFile::getValidFilePath(
    const std::string &filePath) const {
  std::ifstream file;

  file.open(filePath);

  if (file.is_open() == false) throw CantOpenFile(filePath);
  return (file);
}

std::unique_ptr<ConfigFile> ParseConfigFile::getConfigFile() {
  return (std::move(_config));
}
