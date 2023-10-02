/**
 * Creating a socket
 * --
 * A socket is the mechanism that most popular operating systems
 * provide to give programs access to the network. It allows messages
 * to be sent and received between applications (unrelated processes)
 * on different networked machines.
 * 
 * The analogy of creating a socket is that of requesting a telephone
 * line from the phone company.
 * 
 * @fn int socket(int domain, int type, int protocol);
 * @param domain: protocol family the socket will communicate on (AF_INET, AF_INET6, AF_UNIX, AF_LOCAL)
 * @param type: the way data is transmitted (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW)
 * @param protocol: specific protocol to be used (0, IPROTO_TCP, IPROTO_UDP)
 */

#include <iostream>         // std::cout
#include <sys/socket.h>     // socket()
#include <unistd.h>         // close()

#define IPV4    AF_INET
#define IPV6    AF_INET6
#define TCP     SOCK_STREAM
#define UDP     SOCK_DGRAM
#define DEFAULT 0

int main()
{
    int domain, type, protocol;

    domain = IPV4;
    type = TCP;
    protocol = DEFAULT;

    ///////////////////////////////////////////////////////////

    /**
     * @brief create a socket
     * --
     * @param domain: protocol family the socket will communicate on (AF_INET, AF_INET6, AF_UNIX, AF_LOCAL)
     * @param type: the way data is transmitted (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW)
     * @param protocol: specific protocol to be used (0, IPROTO_TCP, IPROTO_UDP)
     */
    int serverSocket = socket(domain, type, protocol);
    if (serverSocket < 0) {
        perror("cannot create socket");
        return EXIT_FAILURE;
    }
    std::cout << "A socket has been created\n";

    ///////////////////////////////////////////////////////////


    close(serverSocket);
    return EXIT_SUCCESS;
}