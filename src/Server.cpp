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

Server::Server(int domain, int service, int protocol, int port, int backlog)
{
    this->setup(domain, service, protocol, port, backlog);
}

Server::~Server() {}

//////////////////////////////////////////////////////////////////////////////////

void
Server::setup(int domain, int service, int protocol, int port, int backlog)
{
    _domain     = domain;
    _service    = service;
    _protocol   = protocol;
    _port       = port;
    _backlog    = backlog;
    _host       = INADDR_ANY;
    _storageDir = "./public/storage";

    // Create socket
    _socket = socket(domain, service, protocol);
    if (_socket < 0)
        throw Server::SocketCreationProblem();
    
    // Enable SO_REUSEADDR option
    #ifdef DEBUG_MODE
        std::cout << CGREEN << "⏻ " << CRESET CBOLD << "Debug mode activated\n" << CRESET;
        std::cout << "  this allows to relaunch server immediately after shutdown\n  thus reusing the same socket before all ressources have been freed\n";
        int reuse = 1;
        if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
            throw Server::SocketCreationProblem();
    #endif
    
    // Bind socket
    std::memset(&_address, 0, sizeof(_address));
    _address.sin_family = _domain;
    _address.sin_port = htons(_port);

    if (inet_pton(AF_INET, "127.0.0.1", &_address.sin_addr) <= 0) {
        std::cerr << "Invalid IP address." << std::endl;
        return ;
    }

    // _address.sin_addr.s_addr = htonl(454); // INADDR_ANY
    if (bind(_socket, (struct sockaddr*)&_address, sizeof(_address)))
        throw Server::SocketBindingProblem();
    
    // start listening
    if (listen(_socket, _backlog) < 0)
        throw Server::ListeningProblem();
    
    // std::system("clear");
    Log::status("Listening on 127.0.0.1:" + ft::to_str(_port));
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