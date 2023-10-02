/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arurangi <arurangi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/30 09:57:23 by arurangi          #+#    #+#             */
/*   Updated: 2023/10/02 19:33:22 by arurangi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
    #define SOCKET_HPP

    #include <iostream>

    #include <stdio.h>
    #include <unistd.h>         // read, write, close
    #include <sys/socket.h>     // socket
    #include <netinet/in.h>
    #include <arpa/inet.h>

    #include "../includes/UserInterface.hpp"

    #define IPV4        AF_INET
    #define TCP         SOCK_STREAM
    #define DEFAULT     0
    #define PORT        80
    #define IP_GOOGLE   "142.250.179.174"
    #define IP_FACEBOOK "179.60.195.36"

    class Socket {

        int                 _socketFD;
        int                 _port;
        struct sockaddr_in  _address;

        int _domain;
        int _type;
        int _protocol;

        Socket() {}
        
        public:
            Socket(int domain, int type, int protocol);
            ~Socket();
            Socket& operator= (Socket&) { return *this; }
            Socket(Socket&) {}
    };

#endif