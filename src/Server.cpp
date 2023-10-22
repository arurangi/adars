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
        http::Response res = this->build_response(req, s._mimeTypes);
        this->send_response(c, res);
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }
}

/*
 * This function goes through the body of a POST request
 * and extracts two informations needed to save data on server: 
 * 1. filename
 * 2. data
*/
void
extract_bodyInfo(http::Request& req, std::string body)
{
    (void)req;
    // Locate Content-Disposition line
    size_t pos;
    if ((pos = body.find("Content-Disposition")) == std::string::npos) {
        Log::error("No `Content-Disposition` in POST request");
        return ; // TODO: handle this better
    }
    std::string s = body.substr(pos);
    std::stringstream ss(s);
    std::string line;
    while (std::getline(ss, line)) {

        // std::cout << CYELLOW << line << CRESET << std::endl;

        // -- advance till `filename`
        if ((pos = line.find("filename=\"")) != std::string::npos)
        {
            std::string filename = line.substr(pos+10, line.size());
            req._filename = filename.substr(0, filename.size()-2); // remove quote at end of line
        } else if ((pos = line.find("Content-Type:")) != std::string::npos) {
            std::getline(ss, line); // skip Content-Type
            std::getline(ss, line); // skip empty line (seperation) 
            req._data += line;           
        } else {
            req._data += line;
        }
    }
    // req._data = line;
}

int
extract_contentLength(std::string request)
{
    std::stringstream ss(request);
    std::string line;
    while (std::getline(ss, line)) {
        std::string keyword = "Content-Length: ";
        if (line.find(keyword) != std::string::npos) {
            std::string length = line.substr(keyword.size(), line.size());
            return std::atoi(length.c_str());
        }
    }
    return 0;
}

http::Request
Server::process_request(const int& client_socket)
{
    http::Request req;
    char buffer[1024];
    std::string request;

    while (true) {
        int bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            throw Server::RequestProcessingIssue();
            break;  // Connection closed or error
        }
        request += std::string(buffer, bytesRead);

        std::cout << CMAGENTA << request << CRESET << std::endl;

        std::stringstream ss(request);
        ss >> req._method >> req._path >> req._version;
        
        req._contentLength = extract_contentLength(request);

        if (req._method != "POST")
            break ;
        
        bytesRead = read(client_socket, buffer, sizeof(buffer));
        if (bytesRead == 0)
            break ;
        else {
            request += std::string(buffer, bytesRead);
        }

        std::cout << CYELLOW << request << CRESET << std::endl;

        // Check for the end of the headers (double CRLF)
        size_t found = request.find("\r\n\r\n");
        if (found != std::string::npos) {
            // Headers found, process request
            std::string requestBody = request.substr(found + 4); // +4 to skip the CRLFCRLF
            // Process requestBody
            extract_bodyInfo(req, requestBody);
            break;
        }
    }

    Log::param("Method", req._method);
    Log::param("Path", req._path);
    Log::param("HTTP version", req._version);
    Log::param("Content-Length", utils::to_str(req._contentLength));
    Log::param("Filename", req._filename);
    Log::param("Data", req._data);
    
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

    // TODO: 
    // GET request, do this
    // POST request, do this
    // DELETE request, do this

    if (req._method == "POST") {
        std::string path = "." + req._path + req._filename;
        std::ofstream outputFile(path, std::ios::binary);
        if (outputFile) {
            outputFile.write(req._data.c_str(), req._data.size());
            outputFile.close();
            std::cout << "Image saved as: " << ("." + req._filename) << std::endl;
        } else {
            std::cerr << "♨ Error saving the image." << std::endl;
        }
        // get_fileTransmittedPage()
        // sendPage_fileUploaded()
        return res;
    }

    /**
     * Figure out which file to open
     * - is it a location, home page?
     * - or a specific file, image or other
     */
    if (req._path == "/") // or other locations
        req._path = "/index.html";
    // std::string dir = get_asset_folder(req._path);
    requestedFile.open("./public" + req._path, std::ios::in);

    if (!requestedFile.is_open()) {
        res.set_status("400", "Bad Request");
        res._body = "";
        res._contentLength = 0;
    } else {
        //////////////////////////////////////////////////////
        // BODY
        // : store content in `response._body`
        res._body = ""; // get_ressource()
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