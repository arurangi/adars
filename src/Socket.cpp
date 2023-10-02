/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arurangi <arurangi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/30 10:06:01 by arurangi          #+#    #+#             */
/*   Updated: 2023/09/30 10:12:38 by arurangi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Socket.hpp"

Socket::Socket(int domain, int type, int protocol)
: _domain(domain), _type(type), _protocol(protocol)
{
    _socketFD = socket(domain, type, protocol);
    std::cout << "socket was created\n";
}