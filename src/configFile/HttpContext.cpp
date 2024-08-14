/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpContext.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/12 11:37:24 by fgabler           #+#    #+#             */
/*   Updated: 2024/08/12 12:01:38 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpContext.hpp"

HttpContext::HttpContext() : _numberOfServers(0) {}

HttpContext::addNewEmptyServer() {
  _serverContext.push_back(std::make_unique<ServerContext>());
  _numberOfServers++;
}
