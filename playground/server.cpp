// Server side C program to demonstrate Socket programming

#include <iostream>

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#define IPV4    AF_INET
#define IPV6    AF_INET6
#define TCP     SOCK_STREAM
#define UDP     SOCK_DGRAM
#define DEFAULT 0
#define SERVER_PORT 8080
#define BACKLOG_LISTENING 3
#define BUFFER_SIZE 1024

#define WEB_PORT 80
#define LOCALHOST "127.0.0.1"

int main()
{
    int                 serverSocket;
    struct sockaddr_in  serverAddress;
    int                 dataExchangeSocket;
    struct sockaddr_in  clientAddress;
    socklen_t           clientAddrLen;


    // Create socket
    serverSocket = socket(IPV4, TCP, DEFAULT);
    if (serverSocket < 0)
    {
        std::perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

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
        clientAddrLen = sizeof(clientAddress);
        if ((dataExchangeSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen)) < 0)
        {
            std::perror("In accept");
            exit(EXIT_FAILURE);
        }

        char buffer[BUFFER_SIZE] = {0};
        int bytes_read = read(dataExchangeSocket, buffer, BUFFER_SIZE);
        std::cout << buffer << std::endl;
        if (bytes_read < 0)
            std::cout << "No bytes are there to read\n";
        
        char response[] = "Hello from the server";
        write(dataExchangeSocket, response, std::strlen(response));
        
        std::cout << "------------------Hello message sent-------------------\n";

        close(dataExchangeSocket);
    }
    return EXIT_SUCCESS;
}