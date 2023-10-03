/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arurangi <arurangi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 19:37:11 by arurangi          #+#    #+#             */
/*   Updated: 2023/10/03 15:52:17 by arurangi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Socket.hpp"

UserInterface ui;

int main()
{
    ui.success("Program is running..");

    // parse config file                            parse.cpp
    // create TCP/IPV4 socket                       init
    // listen and accept incoming connections       connect
    // interprete request, send response            process_request, respond
    // close socket                                 end
    
    return EXIT_SUCCESS;
}