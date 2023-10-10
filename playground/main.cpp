// Server side C program to demonstrate Socket programming

#include "Server.hpp"
#include "Client.hpp"
#include "http.hpp"

#define LOCALHOST "127.0.0.1"
#define PORT 8080
#define BACKLOG 10

class UserInterface {
    public:
        void status(std::string msg) {
            std::cout << CYELLOW << "◌ " << CRESET << msg << std::endl;
        }
};
UserInterface ui;

int main()
{
    Server server; Client client;

    try {
        server.init(IPV4, TCP, DEFAULT, PORT, BACKLOG);
        while (isRunning)
        {
            ui.status("Waiting for new connections..");
            client._socket = accept(server._socket, (struct sockaddr *)&client._address, &client._addrLength);
            if (client._socket < 0)
                std::cout << "Bad Connection\n";
            ui.status("Processing Request");
            server.processRequest(client._socket);
            ui.status("Building Response");
            http::Response r = server.buildResponse();
            ui.status("Sending Response");
            write(client._socket, (r._raw).c_str(), r._contentLength);
            close(client._socket);
        }
    }
    catch (std::exception& e) { // only catches initialization bugs
        std::cout << e.what();
    }
    close(server._socket);
    return EXIT_SUCCESS;
}