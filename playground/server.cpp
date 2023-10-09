// Server side C program to demonstrate Socket programming

#include "Server.hpp"

Server::Server() {}

//////////////////////////////////////////////////////////////////////////////////

void
Server::create(int domain, int service, int protocol, int port)
{
    _domain     = domain;
    _service    = service;
    _protocol   = protocol;
    _port       = port;

    // Create 
    _socket = socket(domain, service, protocol);
    if (_socket < 0)
        throw Server::SocketCreationProblem();
    
    // Enable SO_REUSEADDR option
    #ifdef DEBUG_MODE
        std::cout << "==== Debug mode is ON\n";
        int reuse = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
            throw Server::SocketCreationProblem();
    #endif
    
    // Bind socket
    std::memset(&_address, 0, sizeof(_address));
    _address.sin_family = _domain;
    _address.sin_port = htons(_port);
    _address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_socket, (struct sockaddr*)&_address, sizeof(_address)))
        throw Server::SocketBindingProblem();
}

void
Server::listen(int backlog)
{
    _backlog = backlog;

    if (listen(_socket, _backlog) < 0)
        throw Server::ListeningProblem();
}

/////////////////////////////////////////////////////////////////////////////////////////

void
Server::processRequest(const int& client_socket)
{
    memset(_request._raw, 0, sizeof(_request._raw));
    int bytes_read = read(client_socket, _request._raw, BUFFER_SIZE);
    std::cout << _request._raw << std::endl;
    if (bytes_read < 0)
        std::cout << "No bytes are there to read\n\r";
    
    // process _request [+ check errors]
    std::stringstream ss(_request);
    ss >> _request._method >> _request._path >> _request._version;
}

void Server::sendResponse(onst int& client_socket)
{
    // Server::Request _request

    std::string     response, buffer, body, header;
    std::ifstream   requestedFile("." + _request._path);

    if (!requestedFile.is_open())
        exit(EXIT_FAILURE);

    // get content type (MIME)
    size_t found = file.find(".");
    if (found == std::string::npos)
        std::cout << "No extension found\n";
    std::string extension = file.substr(found+1, file.size());
    std::string _contentType = "text/" + extension;

    // store response body
    while (std::getline(requestedFile, buffer))
        body += buffer + "\n";
    _contentLength = body.size() * BYTES_PER_CHAR;
    requestedFile.close();

    // construct HTTP response
    header   = "HTTP/1.1 200 OK\n";
    header   += "Content-Type: " + _contentType + "\n";
    header   += "Content-Length: " + Utils::to_str(_contentLength) + "\n";
    header   += "Connection: keep-alive\n"; // added for persistent connection

    response = header + "\n" + body;

    write(clientSocket, _http.respond(location).c_str(), _http.getContentLength());
}

int getContentLength() { return _contentLength; }

// Checks no errors happened
void Server::check(int status, std::string error_msg)
{
    if (status < 0)
    {
        std::cout << "Error: " << error_msg << std::endl;
        throw Server::ListeningProblem();
    }
}

////////////////////////////////////////////////////////////////


const char*
Server::SocketCreationProblem::what()
{
    std::cout << "Error: "
              << "server could not CREATE socket"
              << std::endl;
}

const char*
Server::SocketBindingProblem::what()
{
    std::cout << "Error: "
              << "server could not BIND socket"
              << std::endl;
}

const char*
Server::ListeningProblem::what()
{
    std::cout << "Error: "
              << "server had a problem while LISTENING"
              << std::endl;
}