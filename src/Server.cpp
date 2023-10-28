// Server side C program to demonstrate Socket programming

#include "../includes/Server.hpp"

#define DEBUG_MODE

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

    // list images in ./public/images/
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

/**
 * This function parses requests, store useful data in an object,
 * then builds a response based on acquired data. And finally it sends
 * an HTTP response to the client.
 * 
 * questions:
 * - what type of exception need to be raised?
 * - 
*/
void
Server::handle_request(Client& c, Server& s)
{
    try {
        http::Request req = this->process_request(c._socket);
        if (req._method == "POST")
            s.save_payload(req);
        http::Response res = this->build_response(req, s._mimeTypes);
        this->send_response(c, res);
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }
}

void
Server::save_payload(Request& req)
{
    if (req._method == "POST") {
        std::string path = "./public/storage/" + req._filename; // TODO: see config
        std::ofstream outputFile(path, std::ios::binary);
        if (outputFile) {
            outputFile.write(req._payload.c_str(), req._payload.size());
            outputFile.close();
            std::cout << "Image saved as: " << ("." + req._filename) << std::endl;
        } else {
            std::cerr << "♨ Error saving the image." << std::endl;
        }
    }
    req._uri = "/";
}

http::Request
Server::process_request(const int& client_socket)
{
    http::Request req;
    char buffer[1024];
    std::string request;
    size_t found;
    size_t bytesRead;

    bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0)
        throw Server::RequestProcessingIssue();
    request += std::string(buffer, bytesRead);

    Log::simple(request, CMAGENTA);

    req.setStatusLine(request);
    req.setContentLength(request);

    /////// POST REQUESTS //////////

    if (req._method != "POST")
        return req;
    
    req.setReferer(request);

    while (1) {
        bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
            throw Server::RequestProcessingIssue();
        request += std::string(buffer, bytesRead);
        Log::status(utils::to_str(bytesRead) + " bytes read");
        if (bytesRead < sizeof(buffer))
            break ;
    }

    // Log::simple(request, CYELLOW);

    if ((found = request.find(CRLFCRLF)) != std::string::npos) {
        std::string body = request.substr(found + CRLF_SIZE);
        req.setFilename(body);
        req.setPayload(body);
    }

    Log::param("Method", req._method);
    Log::param("Path", req._uri);
    Log::param("HTTP version", req._httpVersion);
    Log::param("Content-Length", utils::to_str(req._contentLength));
    Log::param("Filename", req._filename);
    // Log::param("Data", req._payload);
    
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
    std::ifstream   requestedFile;

    std::string path = req.getPathToRequestedFile();
    // TODO: check if part of allowed paths
    // Log::status("Opening => " + path);
    requestedFile.open(path, std::ios::in);
    if (!requestedFile.is_open()) {
        Log::error("Can't open :" + path);
        perror("Reason: ");
        res.set_status(HTTP_NOT_FOUND);
        requestedFile.open("./public/404.html");
        req._uri = "/404.html";
        if (!requestedFile.is_open()) {
            Log::error("Can't open : ./public/404.html");
            perror("Reason: ");
        }
    }
    //////////////////////////////////////////////////////
    // BODY
    // : store content in `response._body`
    res._body = ""; // get_ressource()
    while (std::getline(requestedFile, buffer))
        res._body += buffer + "\n";
    res._body += '\0';
    res._contentLength = res._body.size();

    // std::cout << CMAGENTA << res._body << CRESET << std::endl;

    requestedFile.close();

    ////////////////////////////////////////////////////
    // STATUS_LINE
    // : version, status_code, status_text
    res._httpVersion = req._httpVersion;
    res._statusLine = res._httpVersion + " " + res._code + " " + res._message + "\r\n";

    ////////////////////////////////////////////////////
    // HEADER:
    // : Content-Type, Content-Length, Connection
    res._header += res._statusLine;
    Log::status("URI: " + req._uri);
    res._contentType = http::get_mime_type(req._uri, mimeType);
    res._header   += "Content-Type: " + res._contentType + "\r\n";
    res._header   += "Content-Length: " + utils::to_str(res._contentLength) + "\r\n";
    res._header   += "Date: " + res.get_gmt_time() + "\r\n";
    res._header   += "Connection: keep-alive\r\n";
    res._header   += "Server: Adars\r\n";
    res._header   += "\r\n";

    // Construct raw HTTP response
    res._raw = res._header + res._body;

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
    std::string line, key, value;
    std::ifstream mimeFile;
    int seperator;

    // open mime file
    mimeFile.open(mimesFilePath, std::ios::in);
    if (!mimeFile.is_open()) {
        Log::error("Error: opening MIME file");
        throw Server::SetupIssue();
    }
    
    // read line by line
    while (std::getline(mimeFile, line)) {
        seperator = line.find(",");
        key = line.substr(0, seperator);
        value = line.substr(seperator+1, line.size());
        tmp[key] = value;
    }
    mimeFile.close();

    return tmp;
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