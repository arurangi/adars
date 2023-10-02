/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arurangi <arurangi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 19:37:11 by arurangi          #+#    #+#             */
/*   Updated: 2023/09/30 00:40:35 by arurangi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../includes/UserInterface.hpp"


#define IPV4        AF_INET
#define TCP         SOCK_STREAM
#define DEFAULT     0

#define IP_GOOGLE   "142.250.179.174"
#define IP_FACEBOOK "179.60.195.36"
#define PORT        80

UserInterface ui;

int main()
{
    // step 1 : create a client socket
    int clientSocket = socket(IPV4, TCP, DEFAULT);

    // step 2 : connect to server socket
    // -- construct the address structure
    struct sockaddr_in address;             // needed in 2nd and 3rd arg of connect()
    address.sin_family = IPV4;
    inet_pton(IPV4, IP_GOOGLE, &address.sin_addr.s_addr);
    address.sin_port = htons(PORT);         // host_to_network_short: converts port number to network byte order
    // -- connect
    int result = connect(clientSocket, (struct sockaddr*)&address, sizeof(address));
    if (result == 0)
        ui.success("Connection was successful");

    // -----------------------------------------

    char message[] = "GET / HTTP/1.1\r\nHost: github.com/arurangi\r\n\r\n";
    send(clientSocket, message, std::strlen(message), DEFAULT);

    char buffer[1024];
    recv(clientSocket, buffer, 1024, DEFAULT);
    std::cout << "Response was:\n"
              << CBLUE << buffer << CRESET << std::endl;
    
    // last
    return 0;
}