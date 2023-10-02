/**
 * Wait for an incoming connection
 * --
 * Before a client can connect to a server, the server should
 * have a socket that is prepared to accept the connections
 *
 */

#include <iostream>         // std::cout(), std::perror()
#include <sys/socket.h>     // socket()
#include <unistd.h>         // close()
#include <netinet/in.h>     // 'struct sockaddr_in'

#define IPV4    AF_INET
#define IPV6    AF_INET6
#define TCP     SOCK_STREAM
#define UDP     SOCK_DGRAM
#define DEFAULT 0

#define SERVER_PORT 8080

#define BACKLOG_LISTENING 3 // randonly chosen

int main()
{
    int                 serverSocket;
    int                 dataExchangeSocket;
    struct sockaddr_in  serverAddress;
    struct sockaddr_in  clientAddress;
    socklen_t           clientAddrLen;


    // Create socket
    serverSocket = socket(IPV4, TCP, DEFAULT);
    if (serverSocket < 0)
    {
        std::perror("cannot create socket");
        exit(EXIT_FAILURE);
    }
    std::cout << "TCP/IPV4 Socket created" << std::endl;

    // Bind socket to the specified address and port
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = IPV4;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)))
    {
        std::perror("bind failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket bound with addres ("
              << INADDR_ANY << ") "
              << "& port (" << SERVER_PORT << ")"
              << std::endl;

    ///////////////////////////////////////////////////////////

    /**
     * @brief Listen() : listen for socket connections and limit the queue of incoming connections
     * ---
     * @param socket : server socket file descripor
     * @param backlog : maximum number of pending connections that can be queued up before connections are refused
     * @return -1 if failed, number > 0 if success
     */
    if (listen(serverSocket, BACKLOG_LISTENING) < 0)
    {
        std::perror("In listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on port " << SERVER_PORT << std::endl;

    /**
     * @brief accept() : accept a new connection on a socket
     * ---
     * @param socket : server socket file descriptor
     * @param address : client address structure to be filled
     * @param address_len : client's address length
     * @return dataExchangeSokcet 
     */
    clientAddrLen = sizeof(clientAddress);
    if ((dataExchangeSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen)) < 0)
    {
        std::perror("In accept");
        exit(EXIT_FAILURE);
    }

    //////////////////////////////////////////////////////////

    // Rest of the code ...

    close(serverSocket);
    return EXIT_SUCCESS;
}