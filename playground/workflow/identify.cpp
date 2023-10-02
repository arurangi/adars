/**
 * Identify the socket
 * --
 * When we talk about naming or identifying a socket, we are talking about
 * assigning a transport address to the socket (a port number
 * in IP networking). In sockets, this operation is called
 * binding an address and the bind system call is used for this.
 * 
 * The analogy is that of assigning a phone number to the line that
 * you requested from the phone company in step 1 or that of
 * assigning an address to a mailbox.
 * 
 */

#include <iostream>         // std::cout(), std::perror()
#include <sys/socket.h>     // socket()
#include <unistd.h>         // close()
#include <netinet/in.h>     // 'struct sockaddr_in'
/*

struct sockaddr_in 
{ 
    __uint8_t         sin_len; 
    sa_family_t       sin_family; 
    in_port_t         sin_port; 
    struct in_addr    sin_addr; 
    char              sin_zero[8]; 
};

*/

#define IPV4    AF_INET
#define IPV6    AF_INET6
#define TCP     SOCK_STREAM
#define UDP     SOCK_DGRAM
#define DEFAULT 0

#define SERVER_PORT 8080

int main()
{
    int                 serverSocket;
    struct sockaddr_in  address;

    // Create socket
    serverSocket = socket(IPV4, TCP, DEFAULT);
    if (serverSocket < 0)
    {
        std::perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    ///////////////////////////////////////////////////////////
    // Before calling bind, we need to fill out the `sockaddr_in` structure
    // The three key parts we need to set are:
    // - sin_family
    // - sin_port
    // - sin_addr

    // Initialize sockaddr_in structure
    memset(&address, 0, sizeof(address));
    address.sin_family = IPV4;
    address.sin_port = htons(SERVER_PORT);          // htons (host_to_network_short) **
    address.sin_addr.s_addr = htonl(INADDR_ANY);    // htonl (host_to_network_long) **

    /* why use `htons` & `htonl` ??
     * ---
     * `htons` and `htonl` are functions used for converting integer values from
     * host byte order to network byte order. They are often used in network
     * programming to ensure that data is transmitted and received consistently
     * across different computer architectures, which may have different
     * byte orderings (endianness). These functions are especially important
     * when dealing with network protocols such as TCP/IP
    */

    // Bind socket to the specified address and port
    /**
     * @param socket: socket file descriptor
     * @param address: structure containing address of the socket (IP & port)
     * @param address_len: size of the address structure
    */
    if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)))
    {
        std::perror("bind failed");
        exit(EXIT_FAILURE);
    }

    ///////////////////////////////////////////////////////////

    // Rest of the code ...

    close(serverSocket);
    return EXIT_SUCCESS;
}