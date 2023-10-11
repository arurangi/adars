// Server side C program to demonstrate Socket programming

#include "Server.hpp"

#define DEBUG_MODE

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

    _mimeTypes = http::store_mime_types();

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
    
    // std::system("clear");
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
    std::cout << _request;
    
    std::stringstream ss(_request._raw);
    ss >> _request._method >> _request._path >> _request._version;
    // check for invalid: method, path and version
}

/** Build HTTP Response
 * **
 * - start-line (http-version, status-code, status-text)
 * - header     (Content-Length, Content-Type, Date)
 * - body       (html, css,..)
*/
Response
Server::buildResponse()
{
    // Server::Request _request
    http::Response  r;
    std::string     buffer;

    // TODO: check if request was processed


    r.set_status("200", "OK");
    r._httpVersion = _request._version;

    ////////////////////////////////////////////////////
    // STATUS_LINE: version, status_code, status_text

    //////////////////////////////////////////////////////
    // BODY

    // handle invalid requests (location)
    if (_request._path != "/index.html" && _request._path != "/styles.css") {
        r.set_status("404", "Not Allowed");
        r._contentLength = 0;
    }
    else
    {
        std::ifstream   requestedFile("." + _request._path);
        if (!requestedFile.is_open())
            r.set_status("400", "Bad Request");
        while (std::getline(requestedFile, buffer))
            r._body += buffer + "\n";
        r._contentLength = r._body.size() * BYTES_PER_CHAR;
        requestedFile.close();
    }

    ////////////////////////////////////////////////////
    // HEADER: Content-Type, Content-Length, Connection
    r._contentType = http::get_mime_type(_request._path, _mimeTypes);
    r._header   += "Content-Type: " + r._contentType + "\n";
    r._header   += "Content-Length: " + Utils::to_str(r._contentLength) + "\n";
    r._header   += "Connection: keep-alive\n"; // added for persistent connection

    /////////////////////////////////////////////////////
    r._statusLine = r._httpVersion + " " + r._code + " " + r._message + "\n";

    // Construct raw HTTP response
    r._raw = r._statusLine + r._header + BLANK_LINE + r._body;

    std::cout << r;
    return r;
}

// Checks no errors happened
void Server::check(int status, string error_msg)
{
    if (status < 0)
    {
        std::cout << "Error: " << error_msg << std::endl;
        throw Server::ListeningProblem();
    }
}

////////////////////////////////////////////////////////////////

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