// Server side C program to demonstrate Socket programming

#include "../includes/Server.hpp"

#define DEBUG_MODE

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Server::Server()
{
    // TODO: allowed paths
    _allowedPaths.push_back("./public/index.html");
    _allowedPaths.push_back("./public/about.html");
    _allowedPaths.push_back("./public/upload.html");
    _allowedPaths.push_back("./public/uploaded.html");
    _allowedPaths.push_back("./public/404.html");
    _allowedPaths.push_back("./public/favicon.ico");
    _allowedPaths.push_back("./public/stylesheets/styles.css");

    std::string bucket =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "<link rel=\"stylesheet\" href=\"stylesheets/styles.css\">\n"
        "<title>Error 404</title>\n"
    "</head>\n"
    "<body>\n"
        "<section class=\"wrapper\">\n"
        "<nav >\n"
            "<ul class=\"navbar\">\n"
            "<li><a href=\"index.html\">Home</a></li>\n"
            "<li><a href=\"about.html\">About</a></li>\n"
            "<li><a href=\"upload.html\">Send files</a></li>\n"
            "<li><a href=\"uploaded.html\">Storage</a></li>\n"
            "</ul>\n"
        "</nav>\n"
            "<h1>Error 404</h1>\n"
        "</section>\n"
    "</body>\n"
    "</html>\n";

    // list images in ./public/images/
}

Server::Server(int domain, int service, int protocol, int port, std::string host, int backlog)
{
    this->setup(domain, service, protocol, port, host, backlog);
}

Server::~Server() {}

//////////////////////////////////////////////////////////////////////////////////

void
Server::setup(int domain, int service, int protocol, int port, std::string host, int backlog)
{
    int status, on = 1;

    _domain     = domain;
    _service    = service;
    _protocol   = protocol;
    _port       = port;
    _backlog    = backlog;
    _host       = host;
    _storageDir = "./public/storage";

    /*************************************************************/
    /* Create an AF_INET4 stream socket to receive incoming      */
    /* connections on                                            */
    /*************************************************************/
    _socket = socket(domain, service, protocol);
    if (_socket < 0)
        exit(Log::out("socket() failed"));
    
    /*************************************************************/
    /* Allow socket descriptor to be reuseable                   */
    /*************************************************************/
    status = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (status < 0) {
        close(_socket);
        exit(Log::out("setsockopt() failed"));
    }

    /*************************************************************/
    /* Set socket to be nonblocking. All of the sockets for      */
    /* the incoming connections will also be nonblocking since   */
    /* they will inherit that state from the listening socket.   */
    /*************************************************************/
    status = ioctl(_socket, FIONBIO, (char *)&on);
    if (status < 0) {
        close(_socket);
        exit(Log::out("ioctl() failed"));
    }
    
    /*************************************************************/
    /* Bind the socket                                           */
    /*************************************************************/
    std::memset(&_address, 0, sizeof(_address));
    _address.sin_family = _domain;
    _address.sin_port = htons(_port);
    status = inet_pton(AF_INET, _host.c_str(), &_address.sin_addr); // TODO: replace IP with server IP
    if (status <= 0) {
        close(_socket);
        exit(Log::out("inet_pton() failed: invalid IP address"));
    }
    // _address.sin_addr.s_addr = htonl(454); // INADDR_ANY
    status = bind(_socket, (struct sockaddr*)&_address, sizeof(_address));
    if (status < 0) {
        close(_socket);
        Log::error("bind() failed: Can't assign requested address");
        exit(-1);
    }
    
    /*************************************************************/
    /* Set the listen back log                                   */
    /*************************************************************/
    status = listen(_socket, _backlog);
    if (status < 0) {
        close(_socket);
        exit(Log::out("listen() failed"));
    }
    
    Log::status("Listening on " + _host + ":" + ft::to_str(_port)); // TODO: add correct address
}

/////////////////////////////////////////////////////////////////////////////////////////

// Checks no errors happened
void Server::check(int status, string error_msg)
{
    if (status < 0)
    {
        std::cout << "Error: " << error_msg << std::endl;
        throw Server::ListeningProblem();
    }
}

/* -------------------------------- Exception ------------------------------- */

const char*
Server::SocketCreationProblem::what() const throw()
{
    return "server could not CREATE socket";
}

const char*
Server::SocketBindingProblem::what() const throw()
{
    return "server could not BIND socket";
}

const char*
Server::ListeningProblem::what() const throw()
{
    return "server had a problem while LISTENING";
}

const char*
Server::SetupIssue::what() const throw()
{
    return "couldn't setup the server";
}

const char*
Server::RequestProcessingIssue::what() const throw()
{
    return "request processing issue";
}