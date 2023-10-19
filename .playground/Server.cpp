// Server side C program to demonstrate Socket programming

#include "Server.hpp"

#define DEBUG_MODE

Server::Server() {}
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

    _mimeTypes = store_mime_types("./conf/mime.types");

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
    _address.sin_addr.s_addr = htonl(_host); // INADDR_ANY
    if (bind(_socket, (struct sockaddr*)&_address, sizeof(_address)))
        throw Server::SocketBindingProblem();
    
    // start listening
    if (listen(_socket, _backlog) < 0)
        throw Server::ListeningProblem();
    
    // std::system("clear");
    Log::status("Listening on 127.0.0.1:8080");
}

/////////////////////////////////////////////////////////////////////////////////////////

int Server::get_client()
{
    struct sockaddr_in  addr;
    socklen_t           addrLen = sizeof(addr);
    int client_socket = accept(this->_socket, (struct sockaddr *)&addr, &addrLen);
    if (client_socket < 0) {
        std::cout << "Bad Connection\n";
        exit(1);
    }
    return client_socket;
}

void
Server::handle_request(Client& c, Server& s) {
    http::Request     req = this->process_request(c._socket); // print
    http::Response    res = this->build_response(req, s._mimeTypes);  // print

    int bytes_sent = 0;
    bytes_sent = send(c._socket, (res._raw).c_str(), res._raw.size(), 0);
    if (bytes_sent < 0)
        std::cout << "in handleHttpRequest(): send(): No bytes to send";

    std::cout << CGREEN << "••• Bytes transmitted: "
            << CBOLD << bytes_sent
            << "/" << res._contentLength << CRESET << std::endl;
            
    close(c._socket);
}

/** Process HTTP Request:
 *      Store all informations in classes/structures for easier access
 * **
 * - start-line (method, path, http-version)
 * - header ()
 * - body (..)
*/
http::Request
Server::process_request(const int& client_socket)
{
    http::Request req;
    int bytes_received;

    memset(req._raw, 0, sizeof(req._raw));
    if ((bytes_received = recv(client_socket, req._raw, BUFFER_SIZE, 0)) < 0)
        std::cout << "No bytes are there to read\n\r";
    req._raw[bytes_received] = '\0';
    std::cout << req;
    std::cout << CBLUE << "••• Bytes received: "
              << CBOLD << bytes_received
              << CRESET << std::endl;
    


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
http::Response
Server::build_response(http::Request& req, std::map<string, string>& mimeType)
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
    if (req._path != "/index.html" && req._path != "/styles.css" && req._path != "/favicon.ico")
    {
        res.set_status("404", "Not Allowed");
        res._contentLength = 0;
    }
    else
    {
        // open requested file
        std::ifstream   requestedFile("." + req._path);
        if (!requestedFile.is_open())
            res.set_status("400", "Bad Request");
        
        // store content in `response._body`
        res._body = "";
        while (std::getline(requestedFile, buffer))
            res._body += buffer + "\n";
        res._body += '\0';
        // calc Content-Length
        res._contentLength = res._body.size();

        requestedFile.close();
    }

    ////////////////////////////////////////////////////
    // HEADER: Content-Type, Content-Length, Connection

    res._statusLine = res._httpVersion + " " + res._code + " " + res._message + "\r\n";

    res._contentType = http::get_mime_type(req._path, mimeType);
    res._header   += "Content-Type: " + res._contentType + "\r\n";
    res._header   += "Content-Length: " + utils::to_str(res._contentLength) + "\r\n";
    res._header   += "Date: " + res.get_gmt_time() + "\r\n";
    res._header   += "Connection: keep-alive\r\n";
    res._header   += "Server: Adars\r\n";
    res._header   += "\r\n";

    // Construct raw HTTP response
    res._raw = res._statusLine + res._header + res._body + "\0";

    if (res._contentType != "image/x-icon")
        std::cout << res;
    else {
        res.set_status("204", "No Content");
    }
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

std::map<std::string, std::string>
Server::store_mime_types(std::string mimesFilePath) {
    std::map<std::string, std::string> tmp;

    // open mime file
    std::ifstream mimeFile(mimesFilePath);
    if (!mimeFile.is_open())
        std::cout << "Error: opening MIME file\n";
    // read line by line
    std::string line;
    while (std::getline(mimeFile, line)) {
        // save in key value pairs in map
        int pos = line.find(",");
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos+1, line.size());
        tmp[key] = value;
    }
    // close mime file
    mimeFile.close();

    return tmp;
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