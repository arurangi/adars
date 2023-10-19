// Server side C program to demonstrate Socket programming

#include "Server.hpp"
#include "Client.hpp"
#include "Http.hpp"
#include <pthread.h>

#define LOCALHOST "127.0.0.1"
#define PORT 8080
#define BACKLOG 10

class UserInterface {
    public:
        void status(std::string msg) {
            std::cout << std::endl << CYELLOW CBOLD
                      << "◌" 
                      << " " << msg << " " << CRESET << std::endl;
        }
};
UserInterface ui;

// void* routine(void* arg) {
//     Server* server = (Server*) arg;

//     Request req = Server::processRequest(server->_cSocket);
//     http::Response res = Server::buildResponse(req, server->_mimeTypes);
//     send(server->_cSocket, (res._raw).c_str(), res._contentLength, 0);
//     close(server->_cSocket);
//     return NULL;
// }

void handleHttpRequest(int client_socket, std::map<string, string> mimeType) {
    Request req = Server::processRequest(client_socket); // print
    Response res = Server::buildResponse(req, mimeType); // print

    int bytes_sent = 0;
    bytes_sent = send(client_socket, (res._raw).c_str(), res._contentLength, 0);
    if (bytes_sent < 0)
        std::cout << "in handleHttpRequest(): send(): No bytes to send";

    std::cout << CGREEN << "••• Bytes transmitted: "
            << CBOLD << bytes_sent
            << "/" << res._contentLength << CRESET << std::endl;
            
    close(client_socket);
}

int main()
{
    Server server; Client client;

    try {
        server.init(IPV4, TCP, DEFAULT, PORT, BACKLOG);
        ui.status("Listening on 127.0.0.1:8080");

        std::vector<std::thread> threads;

        while (isRunning)
        {
            ui.status("Waiting for new connections..");
            client._socket = accept(server._socket, (struct sockaddr *)&client._address, &client._addrLength);
            if (client._socket < 0) {
                std::cout << "Bad Connection\n";
                exit(1);
            }

            // Using threads
            // threads.push_back(std::thread(handleHttpRequest, client._socket, server._mimeTypes));
            // threads.back().detach();

            // Using processes
            int pid = fork();
            if (pid == 0) {
                std::map<string, string> mt = server._mimeTypes;
                close(server._socket);
                handleHttpRequest(client._socket, mt);
                close(client._socket);
                exit(0);
            }
            close(client._socket);
        }
    }
    catch (std::exception& e) { // only catches initialization bugs
        std::cout << e.what();
    }
    close(server._socket);
    return EXIT_SUCCESS;
}