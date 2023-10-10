// Server side C program to demonstrate Socket programming

#include "Server.hpp"

Server::Server() {}
Server::~Server() {}

//////////////////////////////////////////////////////////////////////////////////

void
Server::init(int domain, int service, int protocol, int port, int backlog)
{
    _domain     = domain;
    _service    = service;
    _protocol   = protocol;
    _port       = port;
    _backlog    = backlog;

    _mimeType = http::get_mime_types();

    // Create socket
    _socket = socket(domain, service, protocol);
    if (_socket < 0)
        throw Server::SocketCreationProblem();
    
    // Enable SO_REUSEADDR option
    #ifdef DEBUG_MODE
        std::cout << "==== Debug mode is ON\n";
        int reuse = 1;
        if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
            throw Server::SocketCreationProblem();
    #endif
    
    // Bind socket
    std::memset(&_address, 0, sizeof(_address));
    _address.sin_family = _domain;
    _address.sin_port = htons(_port);
    _address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_socket, (struct sockaddr*)&_address, sizeof(_address)))
        throw Server::SocketBindingProblem();
    
    // start listening
    if (listen(_socket, _backlog) < 0)
        throw Server::ListeningProblem();
}

/////////////////////////////////////////////////////////////////////////////////////////

/** Process HTTP Request:
 *      Store all informations in classes/structures for easier access
 * **
 * - start-line (method, path, http-version)
 * - header ()
 * - body (..)
*/
void
Server::processRequest(const int& client_socket)
{
    // Http::Request _request;

    memset(_request._raw, 0, sizeof(_request._raw));
    if (read(client_socket, _request._raw, BUFFER_SIZE) < 0)
        std::cout << "No bytes are there to read\n\r";
    std::cout << _request._raw << std::endl;
    
    // process _request [+ check errors]
    std::stringstream ss(_request._raw);
    ss >> _request._method >> _request._path >> _request._version;

    // get_content_type()

}

/** Build HTTP Response
 * **
 * - start-line (http-version, status-code, status-text)
 * - header (Content-Length, Content-Type, Date)
 * - body (html, css,..)
*/
Http::Response
Server::buildResponse(const int& client_socket)
{
    // Server::Request _request
    Http::Response rP;

    std::string     buffer, body, header;
    std::ifstream   requestedFile("." + _request._path);

    if (!requestedFile.is_open())
        exit(EXIT_FAILURE);

    // get content type (MIME)
    size_t found = _request._path.find(".");
    if (found == std::string::npos)
        std::cout << "No extension found\n";
    std::string extension = _request._path.substr(found+1, _request._path.size());
    std::string _contentType = "text/" + extension;

    // store response body
    while (std::getline(requestedFile, buffer))
        body += buffer + "\n";
    rP._contentLength = body.size() * BYTES_PER_CHAR;
    requestedFile.close();

    // construct HTTP r
    rP._header   = "HTTP/1.1 200 OK\n";
    rP._header   += "Content-Type: " + _contentType + "\n";
    rP._header   += "Content-Length: " + Utils::to_str(rP._contentLength) + "\n";
    rP._header   += "Connection: keep-alive\n"; // added for persistent connection

    rP._raw = rP._header + "\n" + rP._body;

    return rP;
}

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