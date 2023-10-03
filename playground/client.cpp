// Client side C/C++ program to demonstrate Socket programming

#include <iostream>

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>


#define IPV4        AF_INET

#define LOCALHOST   "127.0.0.1"
#define GOOGLE_IP      
#define FACEBOOK_IP
#define HOST        LOCALHOST

#define DEFAULT     0
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024


int main()
{
    int                 clientSocket;
    struct sockaddr_in  serverAddress;
    long                bytes_read;
    char                message[] = "Hello from client";
    char                buffer[BUFFER_SIZE] = {0};

    // 1. create socket
    clientSocket = socket(IPV4, SOCK_STREAM, DEFAULT);
    if (clientSocket < 0)
    {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }
    
    // 2. connect to remote server
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = IPV4;
    serverAddress.sin_port = htons(SERVER_PORT);
    // -- -- convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(IPV4, LOCALHOST, &serverAddress.sin_addr) <= 0)
    {
        std::cerr << "\nInvalid address/ Address not supported \n";
        exit(EXIT_FAILURE);
    }
    // -- connect
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr << "\nConnection Failed \n";
        exit(EXIT_FAILURE);
    }

    // 3. send message
    send(clientSocket , message , std::strlen(message) , 0);
    std::cout << "Hello message sent\n";

    // 4. read response
    bytes_read = read(clientSocket , buffer, BUFFER_SIZE);
    if(bytes_read < 0)
        printf("No bytes are there to read");
    std::cout << buffer << std::endl;

    return EXIT_SUCCESS;
}