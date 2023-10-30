#include "../includes/Http.hpp"

http::Response::Response() { reset(); }
http::Response::~Response() {}

http::Request::Request() : _contentLength(0), _referer("") {}
http::Request::~Request() { _referer = ""; }
//////////////////////////////////////////////////////////////////////////


void
http::Response::reset()
{
    _statusLine = "";
    _httpVersion = "";
    _code = "200";
    _message = "OK";
    _header = "";
    _contentType = "application/octet-stream";
    _contentLength = 0;
    _body = "";
    _raw = "";


    // This is the standard response for a successful HTTP request. It indicates that the request has been successfully processed, and the response contains the requested information.
    _statusCodes["200"] = "OK";

    // This status code is returned when a new resource has been successfully created as a result of the request. It is often accompanied by a "Location" header that specifies the URL of the newly created resource.
    _statusCodes["201"] = "Created";

    // The request was successfully processed, but there is no content to return in the response body
    _statusCodes["204"] = "No Content";

    // This is one of the most well-known HTTP error codes. It indicates that the requested resource could not be found on the server.
    _statusCodes["404"] = "Not Found";

    // The server understands the request, but it refuses to fulfill it. This might be due to insufficient permissions or access restrictions
    _statusCodes["403"] = "Forbidden";

    // The server cannot understand or process the client's request due to a client error, such as malformed syntax in the request
    _statusCodes["400"] = "Bad Request";

    // The HTTP method used in the request (e.g., GET, POST, PUT) is not allowed for the requested resource.
    _statusCodes["405"] = "Method Not Allowed";

    // When a client sends a conditional GET request and the resource has not been modified, the server responds with this code to indicate that the client's cached version is still valid
    _statusCodes["304"] = "Not Modified";

    // This is a generic error message indicating that something went wrong on the server's side, and the server couldn't be more specific about the issue.
    _statusCodes["500"] = "Internal Server Error";

}

///////////////// UTILS //////////////////////////////////////////////////

int http::accept_connection(int serverSocket)
{
    struct sockaddr_in  addr;
    socklen_t           addrLen = sizeof(addr);
    int client_socket = accept(serverSocket, (struct sockaddr *)&addr, &addrLen);
    if (client_socket < 0) {
        std::cout << "Bad Connection\n";
        exit(1);
    }
    return client_socket;
}

void
http::handle_request(int client_socket)
{
    try {
        Request req = http::parse_request(client_socket);
        if (req._method == "POST")
            http::save_payload(req);
        Response res = http::build_response(req);
        http::send_response(client_socket, res);
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }
}

http::Request
http::parse_request(const int& client_socket)
{
    Request req;
    char buffer[1024];
    string request;
    size_t found;
    size_t bytesRead;

    bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0)
        Log::error("Request processing issues");
    request += string(buffer, bytesRead);

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
            Log::error("Request processing issues");
        request += std::string(buffer, bytesRead);
        Log::status(ft::to_str(bytesRead) + " bytes read");
        if (bytesRead < sizeof(buffer))
            break ;
    }

    // Log::simple(request, CYELLOW);

    if ((found = request.find(CRLFCRLF)) != std::string::npos) {
        string body = request.substr(found + CRLF_SIZE);
        req.setFilename(body);
        req.setPayload(body);
    }

    Log::param("Method", req._method);
    Log::param("Path", req._uri);
    Log::param("HTTP version", req._httpVersion);
    Log::param("Content-Length", ft::to_str(req._contentLength));
    Log::param("Filename", req._filename);
    // Log::param("Data", req._payload);
    
    return req;
}

http::Response
http::build_response(Request& req)
{
    Response res;
    string buffer;
    std::ifstream requestedFile;
    map<string, string> mimeTypes = http::get_mime_types("./conf/mime.types");

    string path = req.getPathToRequestedFile();

    // TODO: check if part of allowed paths
    Log::status("Opening => " + path);

    requestedFile.open(path, std::ios::in);
    if (!requestedFile.is_open()) {
        Log::out("Can't open :" + path);
        res.set_status(HTTP_NOT_FOUND);
        if (AUTOINDEX == ON) {
            req._uri = ".html";
            res._body = http::generate_directoryPage(path);
        }
        else {
            req._uri = "/404.html";
            res._body = generate_errorPage();
        }
    } else {
        //////////////////////////////////////////////////////
        // BODY
        // : store content in `response._body`
        res._body = ""; // get_ressource()
        while (std::getline(requestedFile, buffer)) {
            if (ft::endswith(path, "/uploaded.html") && buffer.find("</body>") != std::string::npos) {
                res._body += generate_storageList();
            }
            res._body += buffer + "\n";
        }
        res._body += '\0';
        requestedFile.close();
    }
    res._contentLength = res._body.size();

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
    res.set_content_type(req._uri, mimeTypes);
    res._header   += "Content-Type: " + res._contentType + "\r\n";
    res._header   += "Content-Length: " + ft::to_str(res._contentLength) + "\r\n";
    res._header   += "Date: " + res.get_gmt_time() + "\r\n";
    res._header   += "Connection: keep-alive\r\n";
    res._header   += "Server: Adars\r\n";
    res._header   += "\r\n";

    // Construct raw HTTP response
    res._raw = res._header + res._body;

    std::cout << res;
    return res;
}

void http::send_response(int client_socket, http::Response& res)
{
    int bytes_sent = 0;
    bytes_sent = send(client_socket, (res._raw).c_str(), res._raw.size(), 0);
    if (bytes_sent < 0)
        Log::error("in handleHttpRequest(): send(): No bytes to send");

    std::cout << CGREEN << "••• Bytes transmitted: "
            << CBOLD << bytes_sent
            << "/" << res._contentLength << CRESET << std::endl;
            
    close(client_socket);
}

void
http::save_payload(Request& req)
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

////////////////////////////////////////////////////////////////////////////

void
http::Response::set_content_type(string filepath, map<string, string> accepted_types)
{
    string defaultMime = "application/octet-stream";

    size_t dotPosition = filepath.find_last_of(".");
    if (dotPosition == string::npos) {
        Log::error("Can't identify MIME type: No extension.");
        _contentType = defaultMime;
    }
    string extension = filepath.substr(dotPosition);
    Log::status("extension:" + extension);

    if (accepted_types.find(extension) != accepted_types.end())
        _contentType = accepted_types[extension];
    // return "text/html";
}

std::map<std::string, std::string>
http::get_mime_types(std::string mimesFilePath) {
    std::map<std::string, std::string> tmp;
    std::string line, key, value;
    std::ifstream mimeFile;
    int seperator;

    // open mime file
    mimeFile.open(mimesFilePath, std::ios::in);
    if (!mimeFile.is_open()) {
        Log::error("Error: opening MIME file");
        // throw exception???
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

std::string http::Response::get_gmt_time()
{
    time_t      rawtime;
    struct tm*  timeinfo;
    char        buffer[80];
    std::string formated_time;

    time(&rawtime);
    timeinfo = gmtime(&rawtime);

    // Define the format string for output with abbreviated day name
    const char* format = "%a, %d %B %Y %H:%M:%S GMT";

    // Format the time and print
    strftime(buffer, sizeof(buffer), format, timeinfo);

    formated_time = buffer;
    return formated_time;
}

std::string
http::generate_directoryPage(string uri)
{
    std::deque<std::string> list = ft::list_files_in(ft::get_dir(uri));
    std::string htmlPage = "";
    htmlPage += "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "<head>\n"
            "<meta charset=\"UTF-8\">\n"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
            "<link rel=\"stylesheet\" href=\"stylesheets/styles.css\">\n"
            "<title>Directory</title>\n"
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
                "</nav>\n";
    while (!list.empty()) {
        htmlPage += "<p>" + list.front() + "</p>\n";
        list.pop_front();
    }
    htmlPage += "</section>"
                "</body>\n"
                "</html>\n";
    
    return htmlPage;
}

std::string
http::generate_errorPage()
{
    // TODO: add map<int, std::string>
    // key (int): status code
    // value (string): full html page of the error code in a string
    std::string page =
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
    return page;
}

std::string
http::generate_storageList()
{
    std::deque<std::string> list = ft::list_files_in("./public/storage");
    std::string storageItem = "";

    while (!list.empty()) {
        storageItem += "<div class=\"file\"><p>";
        storageItem += list.front();
        storageItem += "</p><img src=\"images/delete_icon.png\" height=\"16px\" width=\"16px\"></img></div>\n";
        list.pop_front();
    }
    return storageItem;
}

/* -------------------------------------------------------------------------- */
/*                            OPERATOR OVERLOADING                            */
/* -------------------------------------------------------------------------- */

std::ostream&
operator<< (std::ostream& os, http::Request& rhs)
{
    std::string raw((char*)rhs._raw);
    std::string curr;
    std::stringstream ss(raw);

    os << CBLUE CBOLD << "\n---\n| Request " << CRESET
       << CYELLOW << std::this_thread::get_id() << CRESET << std::endl;
    while (std::getline(ss, curr)) {
        os << CBLUE << "> " << CRESET << curr << std::endl;
    }
    return os;
}

std::ostream&
operator<< (std::ostream& os, http::Response& rhs)
{
    std::string content(rhs._header);
    std::string currentLine;
    std::stringstream ss(content);
    
    while (std::getline(ss, currentLine))
        os << CGREEN << "< " << CRESET << currentLine << std::endl;
    return os;
}

/* -------------------------------------------------------------------------- */
/*                         Response: Member Functions                         */
/* -------------------------------------------------------------------------- */

void
http::Response::set_status(std::string code)
{
    _code = code;
    _message = _statusCodes[code];
}

/* -------------------------------------------------------------------------- */
/*                          Request: Member Functions                         */
/* -------------------------------------------------------------------------- */

void http::Request::setFilename(std::string& body)
{
    size_t pos;
    string line, tmp, keyword = "filename=\"";
    
    if ((pos = body.find("Content-Disposition")) == std::string::npos) {
        Log::error("No `Content-Disposition` in POST request");
        return ;
    }
    std::stringstream ss(body.substr(pos));
    while (std::getline(ss, line)) {
        if ((pos = line.find(keyword)) != std::string::npos) {
            tmp = line.substr(pos+keyword.size(), line.size());
            this->_filename = tmp.substr(0, tmp.size()-2);
            break ;
        }
    }
}

void
http::Request::setPayload(string& body)
{
    size_t pos;
    string line;
    
    if ((pos = body.find("Content-Disposition")) == std::string::npos) {
        Log::error("No `Content-Disposition` in POST request");
        return ;
    }
    std::stringstream ss(body.substr(pos));
    while (std::getline(ss, line)) {
        if ((pos = line.find("filename=\"")) != std::string::npos)
            continue ;
        else if ((pos = line.find("Content-Type:")) != std::string::npos)
        {
            std::getline(ss, line);
            std::getline(ss, line);
            this->_payload += line + LF;
        }
        else
            this->_payload += line + LF;
    }
}

void
http::Request::setStatusLine(std::string& header)
{
    std::stringstream ss(header);
    std::string method, path, version;
    
    ss >> method >> path >> version;

    if (method != "GET" && method != "POST" && method != "DELETE")
        _status = HTTP_BAD_REQUEST;
        

    this->_method = method;
    this->_uri = path;
    this->_httpVersion = version;
}

void
http::Request::setContentLength(std::string& header)
{
    std::stringstream ss(header);
    std::string line;
    while (std::getline(ss, line)) {
        std::string keyword = "Content-Length: ";
        if (line.find(keyword) != std::string::npos) {
            std::string length = line.substr(keyword.size(), line.size());
            this->_contentLength = std::atoi(length.c_str());
            break ;
        }
    }
}

void
http::Request::setReferer(std::string header)
{
    std::stringstream ss(header);
    std::string line;
    size_t found;

    while (std::getline(ss, line)) {
        std::string keyword = "Referer: ";
        if (line.find(keyword) != std::string::npos) {
            if ((found = line.find_last_of("/")) != std::string::npos)
                this->_referer = line.substr(found);
            else
                this->_referer = "/index.html";
            break ;
        }
    }
}

std::string
http::Request::getPathToRequestedFile()
{
    std::string path;
    size_t found = 0;
    std::string storagePath = "/public/storage";
    
    // if ((found = _uri.find(storagePath)) != std::string::npos)
    //     _uri = _uri.substr(found+storagePath.size());

    if ((found = _uri.find("/public")) != std::string::npos)
        path = ".";
    else
        path = "./public";

    if (_method == "GET")
    {
        if (_uri == "/") {
            Log::status(">>> Here");
            _uri = "/index.html";
            path += _uri;
        }
        else
            path += _uri;
    }
    else if (_method == "POST")
    {
        path += "/uploaded.html";
        _uri = path;
    }
    return path;
}