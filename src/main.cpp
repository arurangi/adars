// The brain of the HTTP server

#include "Server.hpp"
#include "Client.hpp"
#include "Http.hpp"

int main()
{
    Server server; Client client;

    try {
        server.setup(IPV4, TCP, DEFAULT, PORT, BACKLOG);

        fd_set current_sockets, ready_sockets; // 2 lists of sockets
        FD_ZERO(&current_sockets); // clear the list
        FD_SET(server._socket, &current_sockets); // add serve socket to list

        while (isRunning)
        {
            // because select is destructive
            ready_sockets = current_sockets;
            if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0) {
                perror("select error");
                exit(EXIT_FAILURE);
            }
            
            for (int i = 0; i < FD_SETSIZE; i++) {
                if (FD_ISSET(i, &ready_sockets)) {
                    // this is a new connection
                    if (i == server._socket) {
                        int client_socket = server.get_client();
                        FD_SET(client_socket, &current_sockets);
                    } else {
                        Client clt(i);
                        server.handle_request(clt, server);
                        FD_CLR(i, &current_sockets);
                    }
                }
            }     
        }
    }
    catch (std::exception& e) { // only catches initialization bugs
        std::cout << e.what();
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