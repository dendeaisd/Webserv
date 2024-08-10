/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/10 12:06:57 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/10 12:22:35 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"
#include <fstream>
#include <string>

void ConfigFile::parsConfigFile(const std::string &fileName)
{
  std::fstream file;
  std::string line;

  file.open(fileName.c_str());
  while (std::getline(file, line) == true)
  {
    
  }
}
