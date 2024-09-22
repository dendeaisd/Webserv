/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 08:13:09 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/11 09:50:26 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

ConfigFile::ConfigFile() {}

void ConfigFile::printConfigFile() {
  std::cout << "-Main-\n"
            << "worker process: [" << _workerProcessesValue << "]" << std::endl
            << "pid: [" << _pidValue << "]" << std::endl
            << "error log: [" << _errorLogValue << "]\n\n";
  _httpContext.printHttpContent();
}
