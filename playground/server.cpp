// Server side C program to demonstrate Socket programming

#include <iostream>

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#include "Http.hpp"

#include <queue>

#include <thread>

#define IPV4                AF_INET
#define IPV6                AF_INET6
#define TCP                 SOCK_STREAM
#define UDP                 SOCK_DGRAM
#define DEFAULT             0
#define SERVER_PORT         8080
#define BACKLOG_LISTENING   3
#define BUFFER_SIZE         1024

#define WEB_PORT            80
#define LOCALHOST           "127.0.0.1"

#define DEBUG_MODE          // comment when not in debug mode

void handleHttpRequest(const int& clientSocket) {

    Http                _http;

    char request[BUFFER_SIZE] = {0};
    memset(request, 0, sizeof(request));
    int bytes_read = read(clientSocket, request, BUFFER_SIZE);
    std::cout << request << std::endl;
    if (bytes_read < 0)
        std::cout << "No bytes are there to read\n\r";
    
    // process request
    std::stringstream ss(request);
    std::string method, location, protocol;
    ss >> method >> location >> protocol;
    std::cout << CYELLOW << location << CRESET << std::endl;

    write(clientSocket, _http.respond(location).c_str(), _http.getContentLength());
}

int main()
{
    int                 serverSocket;
    struct sockaddr_in  serverAddress;
    int                 clientSocket;
    struct sockaddr_in  clientAddress;
    socklen_t           clientAddrLen = sizeof(clientAddress);

    // Create socket
    serverSocket = socket(IPV4, TCP, DEFAULT);
    if (serverSocket < 0)
    {
        std::perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    // Enable SO_REUSEADDR option
    #ifdef DEBUG_MODE
        std::cout << "==== Debug mode is ON\n";
        int reuse = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
    #endif

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = IPV4;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)))
    {
        std::perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections
    if (listen(serverSocket, BACKLOG_LISTENING) < 0)
    {
        std::perror("In listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n";
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen)) < 0)
        {
            std::perror("In accept");
            exit(EXIT_FAILURE);
        }

        // std::thread(handleHttpRequest, clientSocket).detach();

        handleHttpRequest(clientSocket);
        
        std::cout << "------------------Hello message sent-------------------\n";

        close(clientSocket);
    }
    close(serverSocket);
    return EXIT_SUCCESS;
}