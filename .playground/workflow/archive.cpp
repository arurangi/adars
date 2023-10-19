// Server side C program to demonstrate Socket programming

#include "Server.hpp"
#include "Http.hpp"

void handleHttpRequest(const int& clientSocket) {

    Server  server;

    char request[BUFFER_SIZE] = {0};
    std::memset(request, 0, sizeof(request));
    int bytes_read = read(clientSocket, request, BUFFER_SIZE);
    std::cout << request << std::endl;
    if (bytes_read < 0)
        std::cout << "No bytes are there to read\n\r";
    
    // process request
    std::stringstream ss(request);
    std::string method, location, protocol;
    ss >> method >> location >> protocol;
    std::cout << CYELLOW << location << CRESET << std::endl;

    write(clientSocket, server.respond(location).c_str(), server.getContentLength());
}

int main()
{
    int                 serverSocket;
    struct sockaddr_in  serverAddress;
    int                 clientSocket;
    struct sockaddr_in  clientAddress;
    socklen_t           clientAddrLen = sizeof(clientAddress);



    ///////////////////////////////////////////////////////////////
    // Create socket
    ///////////////////////////////////////////////////////////////
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
            std::perror("setsockopt");
            exit(EXIT_FAILURE);
        }
    #endif

    ///////////////////////////////////////////////////////////////
    // Bind socket
    ///////////////////////////////////////////////////////////////
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = IPV4;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)))
    {
        std::perror("bind failed");
        exit(EXIT_FAILURE);
    }

    http::bindSocket(&serverSocket, &serverAddress);

    ///////////////////////////////////////////////////////////////
    // listen for incoming connections
    ///////////////////////////////////////////////////////////////
    if (listen(serverSocket, BACKLOG_LISTENING) < 0)
    {
        std::perror("In listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n";
        ///////////////////////////////////////////////////////////////
        // accept connection
        ///////////////////////////////////////////////////////////////
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen)) < 0)
        {
            std::perror("In accept");
            exit(EXIT_FAILURE);
        }

        ///////////////////////////////////////////////////////////////
        // handle request
        ///////////////////////////////////////////////////////////////
        handleHttpRequest(clientSocket);
        std::cout << "------------------Hello message sent-------------------\n";

        close(clientSocket);
    }
    close(serverSocket);
    return EXIT_SUCCESS;
}