// Server side C program to demonstrate Socket programming

#include "Server.hpp"
#include "Client.hpp"
#include "Http.hpp"

#define LOCALHOST "127.0.0.1"
#define PORT 8080
#define BACKLOG 10

int main()
{
    Server server; Client client;

    // parse config file
    // create and launch server
    try {
        // create socket and start listening for incoming connections
        server.init(IPV4, TCP, DEFAULT, PORT, BACKLOG);
        // 
        while (isRunning)
        {
            client._socket = accept(server._socket, (struct sockaddr *)&client._address, &client._addrLength);
            if (client._socket < 0)
                throw Server::BadConnection();
            server.processRequest(client._socket);
            http::Response r = server.buildResponse(client._socket);
            write(client._socket, r._raw, r._contentLength);
            close(client._socket);
        }
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }
    close(server._socket);
    return EXIT_SUCCESS;
}