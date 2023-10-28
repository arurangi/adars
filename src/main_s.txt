// Server side C program to demonstrate Socket programming

#include "Server.hpp"
#include "Client.hpp"
#include "Http.hpp"
#include <pthread.h>

#define LOCALHOST "127.0.0.1"
#define PORT 8080
#define BACKLOG 10


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


int accept_new_connection(int server_socket){
    struct sockaddr_in  addr;
    socklen_t           addrLen = sizeof(addr);
    int client_socket = accept(server_socket, (struct sockaddr *)&addr, &addrLen);
    if (client_socket < 0) {
        std::cout << "Bad Connection\n";
        exit(1);
    }
    return client_socket;
}

int main()
{
    Server server; Client client;

    try {
        server.setup(IPV4, TCP, DEFAULT, PORT, BACKLOG);
        ui.status("Listening on 127.0.0.1:8080");

        std::vector<std::thread> threads;

        // initialize current set
        fd_set current_sockets, ready_sockets;
        FD_ZERO(&current_sockets);
        FD_SET(server._socket, &current_sockets);

        while (isRunning)
        {
            // because select is destructive
            ready_sockets = current_sockets;

            if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
            {
                perror("select error");
                exit(EXIT_FAILURE);
            }

            // when select returns, we know that one of our files has some work to do
            // traverse all saved file descriptors
            for (int i = 0; i < FD_SETSIZE; i++) {
                if (FD_ISSET(i, &ready_sockets)) {
                    // this is a new connection
                    int client_socket = accept_new_connection(server._socket);
                    FD_SET(client_socket, &current_sockets);
                } else {
                    std::map<string, string> mt = server._mimeTypes;
                    handleHttpRequest(i, mt);
                    FD_CLR(i, &current_sockets);
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