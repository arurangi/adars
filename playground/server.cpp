// Server side C program to demonstrate Socket programming

#include "Server.hpp"

#define DEBUG_MODE

Server::Server() : _cAddrLength(sizeof(_cAddr)) {}
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
http::Request
Server::processRequest(const int& client_socket)
{
    http::Request req;

    memset(req._raw, 0, sizeof(req._raw));
    if (recv(client_socket, req._raw, BUFFER_SIZE, 0) < 0)
        std::cout << "No bytes are there to read\n\r";
    std::cout << req;

    std::stringstream ss(req._raw);
    ss >> req._method >> req._path >> req._version;
    // check for invalid: method, path and version

    return req;
}

/** Build HTTP Response
 * **
 * - start-line (http-version, status-code, status-text)
 * - header     (Content-Length, Content-Type, Date)
 * - body       (html, css,..)
*/
Response
Server::buildResponse(Request req, std::map<string, string> mimeType)
{
    http::Response  res;
    std::string     buffer;

    // TODO: check if request was processed

    res.set_status("200", "OK");
    res._httpVersion = req._version;

    ////////////////////////////////////////////////////
    // STATUS_LINE: version, status_code, status_text

    //////////////////////////////////////////////////////
    // BODY

    // TODO: make it dynamic instead of manually adding files
    // handle invalid requests (location)
    if (req._path != "/index.html" && req._path != "/styles.css" && req._path != "/favicon.ico") {
        res.set_status("404", "Not Allowed");
        res._contentLength = 0;
    }
    else
    {
        std::ifstream   requestedFile("." + req._path);
        if (!requestedFile.is_open())
            res.set_status("400", "Bad Request");
        while (std::getline(requestedFile, buffer))
            res._body += buffer + "\n";
        res._contentLength = res._body.size() * BYTES_PER_CHAR;
        requestedFile.close();
    }

    ////////////////////////////////////////////////////
    // HEADER: Content-Type, Content-Length, Connection

    res._contentType = http::get_mime_type(req._path, mimeType);
    res._header   += "Content-Type: " + res._contentType + "\n";
    res._header   += "Content-Length: " + Utils::to_str(res._contentLength) + "\n";
    res._header   += "Connection: keep-alive\n"; // added for persistent connection

    /////////////////////////////////////////////////////
    res._statusLine = res._httpVersion + " " + res._code + " " + res._message + "\n";

    // Construct raw HTTP response
    res._raw = res._statusLine + res._header + BLANK_LINE + res._body;

    std::cout << res;
    return res;
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