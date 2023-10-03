/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arurangi <arurangi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 19:37:11 by arurangi          #+#    #+#             */
/*   Updated: 2023/10/03 08:24:00 by arurangi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Socket.hpp"

UserInterface ui;

int main()
{
    ui.success("Running..");

    // parse config file                            parse.cpp
    // create TCP/IPV4 socket                       init
    // listen and accept incoming connections       connect
    // interprete request, send response            handle_request
    // close socket                                 end
    
    return EXIT_SUCCESS;
}