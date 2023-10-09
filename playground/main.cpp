// Server side C program to demonstrate Socket programming

#include "Server.hpp"
#include "Client.hpp"
#include "Http.hpp"

#define LOCALHOST   "127.0.0.1"
#define PORT    8080
#define BACKLOG 10

int main()
{
    Server server;
    Client client;

    try {
        server.create(IPV4, TCP, DEFAULT, PORT);
        server.listen(BACKLOG);
        while (isRunning)
        {
            client._socket = accept(server._socket, (struct sockaddr *)&client._address, &client._addrLength);
            Server::check(client._socket, "Could not accept connection");
            server.processRequest(client._socket);
            server.sendResponse(client._socket);
            close(client._socket);
        }
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }
    close(server._socket);
    return EXIT_SUCCESS;
}