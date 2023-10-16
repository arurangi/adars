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

// void* handleHttpRequest(Server* server) {
//     server->processRequest(server->_cSocket);
//     http::Response r = server->buildResponse();
//     write(server->_cSocket, (r._raw).c_str(), r._contentLength);
//     close(server->_cSocket);
//     return NULL;
// }

void* routine(void* arg) {
    Server* server = (Server*) arg;

    server->processRequest(server->_cSocket);
    http::Response r = server->buildResponse();
    write(server->_cSocket, (r._raw).c_str(), r._contentLength);
    close(server->_cSocket);
    return NULL;
}

int main()
{
    Server server; Client client;

    try {
        server.init(IPV4, TCP, DEFAULT, PORT, BACKLOG);
        while (isRunning)
        {
            ui.status("Waiting for new connections..");
            server._cSocket = accept(server._socket, (struct sockaddr *)&server._cAddr, &server._cAddrLength);
            if (server._cSocket < 0)
                std::cout << "Bad Connection\n";

            // create thread to process request and send response
            pthread_t thread;
            pthread_create(&thread, NULL, &routine, (void *)&server);
            // pthread_detach(thread);
        }
    }
    catch (std::exception& e) { // only catches initialization bugs
        std::cout << e.what();
    }
    close(server._socket);
    return EXIT_SUCCESS;
}