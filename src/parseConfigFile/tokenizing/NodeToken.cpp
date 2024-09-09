/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NodeToken.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgabler <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:20:25 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/03 14:21:20 by fgabler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "NodeToken.hpp"

TokenNode::TokenNode()
    : _type(TypeToken::DEFAULT), _foundLine("0"), _semikolonSet(false) {}
