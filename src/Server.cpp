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
Server::handle_request(Client& c, Server& s)
{
    http::Request req = this->process_request(c._socket);
    http::Response res = this->build_response(req, s._mimeTypes);
    this->send_response(c, res);
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
    int bytes_received = 0;

    // READ HEADER
    memset(req._raw, 0, sizeof(req._raw));
    if ((bytes_received = recv(client_socket, req._raw, BUFFER_SIZE, 0)) <= 0)
        std::cout << "No bytes are there to read\n\r";
    req._raw[bytes_received] = '\0';
    // -- save status line (method, path, HTTP version)
    std::stringstream ss(req._raw);
    ss >> req._method >> req._path >> req._version;

    /////////////////////////////////////////////////// TODO
    // LOOK FOR CONTENT-LENGTH
    // -- find line at which Content-Length is
    //      -- skip the title
    //      -- convert number string to int
    // 
    std::cout << req << std::endl;

    if (req._method == "POST")
    {
        std::string header(req._raw);
        std::string line;
        while (std::getline(ss, line)) {

            if (line.find("Content-Length: ") != std::string::npos) {
                std::string length = line.substr(16, line.size());
                req._contentLength = std::atoi(length.c_str());
                // std::cout << CMAGENTA << "Length as integer: " << req._contentLength << CRESET << std::endl;
            }
        }

        // TODO: exception when do not find content-length

        int bytesRead = 0, totalBytes = 0;
        while (totalBytes < req._contentLength)
        {
            bytesRead = recv(client_socket, req._body, BUFFER_SIZE, 0);
            if (bytesRead < 0) {
                std::cout << CRED << "Error while reading POST request\n" << CRESET << std::endl;
                exit(1);
            }
            totalBytes += bytesRead;
        }
        req._body[totalBytes-1] = '\0';
        std::cout << "///////////////////////\n";
        std::cout << CMAGENTA << req._body << CRESET << std::endl;
        std::cout << "///////////////////////\n";
        
    }
    ////////////////////////////////////////////////// TODO

    // READ BODY

    // TODO: 
    // - method is GET, POST or DELETE
    // - path is not empty
    // - version is HTTP:1.1 or HTTP:1.2
    return req;
}

/**
 * get_asset_folder()
 * : in order to access ressources, I need to figure out in which folder they are placed
 */
std::string get_asset_folder(std::string path) {

    int pos = 0;
    std::string extension;

    for (pos = path.size()-1; pos > 0; pos--) {
        if (path[pos] == '.')
            break ;
    }
    if (pos == 0)
        return "./public";
    extension = path.substr(pos, path.size());
    
    if (extension == ".css")
        return "./public/stylesheets";
    else if (extension == ".svg" || extension == ".jpeg" || extension == "jpg" || extension == ".png" || extension == ".ico")
        return "./public/images";
    else
        // either file hasn't extension above, or not extension, or wrong extension
        return "./public";
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
    std::ifstream   requestedFile;

    // TODO: 
    // GET request, do this
    // POST request, do this
    // DELETE request, do this

    // if (req._method != "GET") {
    //     Log::status("POST method hasn't been implemented yet.");
    //     exit(0);
    // }

    /**
     * Figure out which file to open
     * - is it a location, home page?
     * - or a specific file, image or other
     */
    if (req._path == "/") // or other locations
        req._path = "/index.html";
    // std::string dir = get_asset_folder(req._path);
    requestedFile.open("./public" + req._path, std::ifstream::in);

    if (!requestedFile.is_open()) {
        res.set_status("400", "Bad Request");
        res._body = "";
        res._contentLength = 0;
    } else {
        //////////////////////////////////////////////////////
        // BODY
        // : store content in `response._body`
        res._body = "";
        while (std::getline(requestedFile, buffer))
            res._body += buffer + "\n";
        res._body += '\0';
        res._contentLength = res._body.size();

        requestedFile.close();
    }

    ////////////////////////////////////////////////////
    // STATUS_LINE
    // : version, status_code, status_text
    res._httpVersion = req._version;
    res._statusLine = res._httpVersion + " " + res._code + " " + res._message + "\r\n";

    ////////////////////////////////////////////////////
    // HEADER:
    // : Content-Type, Content-Length, Connection
    res._contentType = http::get_mime_type(req._path, mimeType);
    res._header   += "Content-Type: " + res._contentType + "\r\n";
    res._header   += "Content-Length: " + utils::to_str(res._contentLength) + "\r\n";
    res._header   += "Date: " + res.get_gmt_time() + "\r\n";
    res._header   += "Connection: keep-alive\r\n";
    res._header   += "Server: Adars\r\n";
    res._header   += "\r\n";

    // Construct raw HTTP response
    res._raw = res._statusLine + res._header + res._body;

    std::cout << res;
    return res;
}

void Server::send_response(Client& c, http::Response& res)
{
    int bytes_sent = 0;
    bytes_sent = send(c._socket, (res._raw).c_str(), res._raw.size(), 0);
    if (bytes_sent < 0)
        std::cout << "in handleHttpRequest(): send(): No bytes to send";

    std::cout << CGREEN << "••• Bytes transmitted: "
            << CBOLD << bytes_sent
            << "/" << res._contentLength << CRESET << std::endl;
            
    close(c._socket);
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