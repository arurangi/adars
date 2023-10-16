// Server side C program to demonstrate Socket programming

#include "Server.hpp"
#include "Client.hpp"
#include "http.hpp"
#include <pthread.h>

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

void* routine(void* arg) {
    Server* server = (Server*) arg;

    Request req = Server::processRequest(server->_cSocket);
    http::Response res = Server::buildResponse(req, server->_mimeTypes);
    send(server->_cSocket, (res._raw).c_str(), res._contentLength, 0);
    close(server->_cSocket);
    return NULL;
}

void handleHttpRequest(int client_socket, std::map<string, string> mimeType) {
    Request req = Server::processRequest(client_socket); // print
    Response res = Server::buildResponse(req, mimeType); // print
    send(client_socket, (res._raw).c_str(), res._contentLength, 0);
    close(client_socket);
}

int main()
{
    Server server; Client client;

    try {
        server.init(IPV4, TCP, DEFAULT, PORT, BACKLOG);

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
                ui.status("Created a process");
                std::map<string, string> mt = server._mimeTypes;
                close(server._socket);
                handleHttpRequest(client._socket, mt);
                close(client._socket);
                ui.status("Killing process");
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