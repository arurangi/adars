// The brain of the HTTP server

#include "Server.hpp"
#include "Client.hpp"
#include "Http.hpp"

int main()
{
    Server server; Client client;

    try {
        server.setup(IPV4, TCP, DEFAULT, PORT, BACKLOG);
        // TODO:
        // - cluster of servers listening on different ports
        // - NON-BLOCKING when handling multiple clients
        while (isRunning)
        {
            client._socket = server.get_client();
            server.handle_request(client, server);
            close(client._socket);
        }
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }
    close(server._socket);
    return EXIT_SUCCESS;
}