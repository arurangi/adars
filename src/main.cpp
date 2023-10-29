// The brain of the HTTP server

#include "../includes/Server.hpp"
#include "Client.hpp"
#include "Http.hpp"
#include "../includes/Cluster.hpp"


int main()
{
    Cluster cluster;

    try {
        cluster.init();
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }

    fd_set current_sockets, ready_sockets; // 2 lists of sockets
    FD_ZERO(&current_sockets); // clear the list
    for (IteratorS it = cluster.begin(); it != cluster.end(); it++)
        FD_SET((*it).second._socket, &current_sockets); // add serve socket to list
    while (isRunning)
    {
        // because select is destructive
        ready_sockets = current_sockets;
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
            exit(Log::out("select error"));
        
        for (int curr_fd = 0; curr_fd < FD_SETSIZE; curr_fd++) {
            if (FD_ISSET(curr_fd, &ready_sockets)) {
                // this is a new connection
                int found = cluster.find(curr_fd);
                if (found) {
                    int new_clientSocket = http::accept_connection(found);
                    FD_SET(new_clientSocket, &current_sockets);
                } else {
                    Client existing_client(curr_fd);
                    http::handle_request(existing_client, server);
                    FD_CLR(curr_fd, &current_sockets);
                }
            }
        }
    }

    close(server._socket);
    return EXIT_SUCCESS;
}

// int main()
// {
//     Server server; Client client;

//     try {
//         server.setup(IPV4, TCP, DEFAULT, PORT, BACKLOG);
//         // TODO:
//         // - cluster of servers listening on different ports
//         // - NON-BLOCKING when handling multiple clients
//         while (isRunning)
//         {
//             client._socket = server.get_client();
//             server.handle_request(client, server);
//             close(client._socket);
//         }
//     }
//     catch (std::exception& e) {
//         std::cout << e.what();
//     }
//     close(server._socket);
//     return EXIT_SUCCESS;
// }