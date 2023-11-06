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
    if (client_socket < 0)
        throw http::AcceptFailed();
    return client_socket;
}

void
http::handle_request(int client_socket, Cluster& cluster)
{
    Request req = http::parse_request(client_socket);
    Server server = cluster.getServerByPort(req._server_port);
    req.execute(server.get_storage_dir());
    Response res = http::build_response(req, server);
    http::send_response(client_socket, res);
}

void
http::Request::execute(string storageDir) {
    if (_method == "POST" && _contentLength > 0)
        save_payload(storageDir);
    if (_method == "DELETE") {
        string filename = _queryParameters["name"];
        string filepath = storageDir + filename;
        if (ft::startswith(filepath, "./public/")) {
            Log::status(">>>>>>>>> deleting file at: " + filepath + "<<<<<<<<<<<\n");
            remove(filepath.c_str());
        }
    }
}

http::Request
http::parse_request(const int& client_socket)
{
    Request req;
    char    buffer[1024];
    string  request;
    size_t  found;
    size_t  bytesRead;

    bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytesRead < 0)
        throw http::ReceiveFailed();
    request += string(buffer, bytesRead);
    req.set_headerInfos(request);

    req._header = request;

    /**
     * At this point, the request can either be GET, POST or DELETE
     * - check if it's one of those
     * - check if that request is allowed on the server
     * - handle each request accordingly:
     *      - GET
     *      - POST
     *      - DELETE
    */

    std::cout << req;

    if ((found = req._uri.find_first_of("?")) != string::npos)
        req.parse_query();

    // Log::simple(request, CMAGENTA);
    // Log::param("Method", req._method);
    // Log::param("Path", req._uri);
    // Log::param("HTTP version", req._httpVersion);
    // Log::param("Content-Length", ft::to_str(req._contentLength));
    // Log::param("Server port", ft::to_str(req._server_port));
    // Log::param("Filename", req._filename);

    // exit(0);

    if (req._method != "POST")
        return req;

    /////// POST REQUESTS //////////
    
    req.setReferer(request);

    while (1) {
        bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesRead < 0)
            throw http::ReceiveFailed();
        request += string(buffer, bytesRead);
        // Log::status(ft::to_str(bytesRead) + " bytes read");
        if (bytesRead < sizeof(buffer))
            break ;
    }

    // Log::simple(request, CYELLOW);

    if ((found = request.find(CRLFCRLF)) != string::npos) {
        string body = request.substr(found + CRLF_SIZE);
        req.setFilename(body);
        req.setPayload(body);
    }

    // Log::param("Method", req._method);
    // Log::param("Path", req._uri);
    // Log::param("HTTP version", req._httpVersion);
    // Log::param("Content-Length", ft::to_str(req._contentLength));
    // Log::param("Filename", req._filename);
    // Log::param("Data", req._payload);
    
    return req;
}

http::Response
http::build_response(Request& req, Server& server)
{
    Response            res;
    string              buffer;
    std::ifstream       requestedFile;
    map<string, string> mimeTypes = http::get_mime_types("./conf/mime.types");
    (void)server;

    /**
     * At this point, I've gathered all informations about the request.
     * Now possibilities:
     * 1) Bad request => return default error page for that location
     * 2) Good request => build appropriate response
     *      - GET --> 2 cases
     *          -- standard: 
     *          -- query_parameters: 
     *      - POST --> send uploaded.html
     *      - DELETE --> send uploaded.html
    */

    Log::ltree(server.get_locations());

    // Log::status("Building response from server: " + server.get_server_name());

    string path = req.getPathToRequestedFile();

    // TODO: check if part of allowed paths
    // Log::status("Opening => " + path);

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
            if (ft::endswith(path, "/uploaded.html") && buffer.find("</body>") != string::npos) {
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
    // Log::status("URI: " + req._uri);
    res.set_content_type(req._uri, mimeTypes);
    res._header   += "Content-Type: " + res._contentType + "\r\n";
    res._header   += "Content-Length: " + ft::to_str(res._contentLength) + "\r\n";
    res._header   += "Date: " + res.get_gmt_time() + "\r\n";
    res._header   += "Connection: keep-alive\r\n";
    res._header   += "Server: Adars\r\n";
    res._header   += "Cache-Control: no-cache\r\n";
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

    // std::cout << CGREEN << "••• Bytes transmitted: "
    //         << CBOLD << bytes_sent
    //         << "/" << res._contentLength << CRESET << std::endl;
            
    close(client_socket);
}

void
http::Request::save_payload(string storageDir)
{
    Log::status("save_payload()..");
    if (_method == "POST") {
        string path = storageDir + _filename; // TODO: prefix with _storage_dir
        Log::status("filepath: " + path);
        std::ofstream outputFile(path, std::ios::binary);
        if (outputFile) {
            outputFile.write(_payload.c_str(), _payload.size());
            outputFile.close();
            std::cout << "Image saved as: " << ("." + _filename) << std::endl;
        } else {
            std::cerr << "♨ Error saving the image." << std::endl;
        }
    }
    _uri = "/";
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

    if (accepted_types.find(extension) != accepted_types.end())
        _contentType = accepted_types[extension];
    // return "text/html";
}

std::map<string, string>
http::get_mime_types(string mimesFilePath) {
    std::map<string, string> tmp;
    string line, key, value;
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

string http::Response::get_gmt_time()
{
    time_t      rawtime;
    struct tm*  timeinfo;
    char        buffer[80];
    string formated_time;

    time(&rawtime);
    timeinfo = gmtime(&rawtime);

    // Define the format string for output with abbreviated day name
    const char* format = "%a, %d %B %Y %H:%M:%S GMT";

    // Format the time and print
    strftime(buffer, sizeof(buffer), format, timeinfo);

    formated_time = buffer;
    return formated_time;
}

string
http::generate_directoryPage(string uri)
{
    std::deque<string> list = ft::list_files_in(ft::get_dir(uri));
    string htmlPage = "";
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

string
http::generate_errorPage()
{
    // TODO: add map<int, string>
    // key (int): status code
    // value (string): full html page of the error code in a string
    string page =
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

std::set<string> get_xtension_list(std::deque<string> files_list)
{
    std::set<string> xtensions;
    while (!files_list.empty()) {
        string curr_file = files_list.front();
        size_t dotPosition = curr_file.find_last_of(".");
        if (dotPosition != string::npos) {
            xtensions.insert(curr_file.substr(dotPosition));
            Log::mark(curr_file.substr(dotPosition));
        }
        files_list.pop_front();
    }
    return xtensions;
}

string
http::generate_storageList()
{
    std::deque<string> list = ft::list_files_in("./public/storage");
    string storageItem = "";

    // filtering buttons
    // std::set<string> xtension_list = get_xtension_list(list);
    // storageItem += "<div class=\"filter_buttons\">\n";
    // storageItem += "<p>filter by: </p>\n";
    // storageItem += html::filter("all");

    // storageItem += "<form action=\"filter\" method=\"get\"> \
    //                 <input type=\"submit\" name=\"all\" value=\"all\" /> \
    //                 </form>";


    // storageItem += "<button class=\"toggle\">all</button>\n";
    // set<string>::iterator itr = xtension_list.begin();
    // for (; itr != xtension_list.end(); itr++)
    //     storageItem += "<button class=\"toggle\">" + (*itr) + "</button>\n";
    // storageItem += "</div>\n";

    // files list
    while (!list.empty()) {
        storageItem += "<div class=\"file\">\n";
        storageItem += "<p>" + list.front() + "</p>";
        storageItem += "<img onclick='deleter(\"" + list.front() + "\")' src=\"images/delete_icon.png\" height=\"16px\" width=\"16px\">\n";
        storageItem += "</div>\n";
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
    string curr;
    stringstream ss(rhs._header);

    os << CBLUE CBOLD << "\nREQUEST\n" << CRESET;
    while (std::getline(ss, curr)) {
        os << CBLUE CBOLD << ">> " << CRESET << curr << std::endl;
    }
    return os;
}

std::ostream&
operator<< (std::ostream& os, http::Response& rhs)
{
    string content(rhs._header);
    string currentLine;
    stringstream ss(content);
    
    os << CGREEN CBOLD << "\nRESPONSE\n" << CRESET; 
    while (std::getline(ss, currentLine))
        os << CGREEN << "<< " << CRESET << currentLine << std::endl;
    return os;
}

/* -------------------------------------------------------------------------- */
/*                         Response: Member Functions                         */
/* -------------------------------------------------------------------------- */

void
http::Response::set_status(string code)
{
    _code = code;
    _message = _statusCodes[code];
}

/* -------------------------------------------------------------------------- */
/*                          Request: Member Functions                         */
/* -------------------------------------------------------------------------- */

void http::Request::setFilename(string& body)
{
    size_t pos;
    string line, tmp, keyword = "filename=\"";
    
    if ((pos = body.find("Content-Disposition")) == string::npos) {
        Log::error("No `Content-Disposition` in POST request");
        return ;
    }
    stringstream ss(body.substr(pos));
    while (std::getline(ss, line)) {
        if ((pos = line.find(keyword)) != string::npos) {
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
    
    if ((pos = body.find("Content-Disposition")) == string::npos) {
        Log::error("No `Content-Disposition` in POST request");
        return ;
    }
    stringstream ss(body.substr(pos));
    while (std::getline(ss, line)) {
        if ((pos = line.find("filename=\"")) != string::npos)
            continue ;
        else if ((pos = line.find("Content-Type:")) != string::npos)
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
http::Request::set_headerInfos(string& header_raw)
{
    stringstream ss(header_raw);
    string method, path, version, line;
    
    // status line
    ss >> method >> path >> version;

    if (method != "GET" && method != "POST" && method != "DELETE") {
        if (path.empty() && version.empty())
            throw EmptyRequest();
        _status = HTTP_BAD_REQUEST;
    }

    this->_method = method;
    this->_uri = path;
    this->_httpVersion = version;

    stringstream ss2(header_raw);

    // header
    while (std::getline(ss2, line)) {
        string keyword = "Content-Length: ";
        if (line.find(keyword) != string::npos) {
            string length = line.substr(keyword.size(), line.size());
            Log::error(length);
            this->_contentLength = std::atoi(length.c_str());
        }
        keyword = "Host: ";
        if (line.find(keyword) != string::npos) {
            string host = line.substr(keyword.size(), line.size());
            string port = host.substr(host.find_last_of(":")+1);
            this->_server_port = std::atoi(port.c_str());
        }

    }

}

void
http::Request::setStatusLine(string& header)
{
    stringstream ss(header);
    string method, path, version;
    
    ss >> method >> path >> version;

    if (method != "GET" && method != "POST" && method != "DELETE")
        _status = HTTP_BAD_REQUEST;

    this->_method = method;
    this->_uri = path;
    this->_httpVersion = version;
}

void
http::Request::setContentLength(string& header)
{
    stringstream ss(header);
    string line;
    while (std::getline(ss, line)) {
        string keyword = "Content-Length: ";
        if (line.find(keyword) != string::npos) {
            string length = line.substr(keyword.size(), line.size());
            this->_contentLength = std::atoi(length.c_str());
            break ;
        }
    }
}

void    http::Request::parse_query()
{
    size_t pos;
    string query, parameter, key, value, toReplace = "%20";
    stringstream ss;

    pos = _uri.find_first_of("?");
    query = _uri.substr(
        pos+1,
        (_uri.find("#") != string::npos) ? _uri.find_first_of("#") : _uri.size()
    );
    _uri = _uri.substr(0, pos); // remove query string from URI

    // remove %20 in query string
    while ((pos = query.find(toReplace)) != string::npos)
        query.replace(pos, toReplace.length(), " ");

    // store query parameters in Request object
    ss << query;
    while (std::getline(ss, parameter, '&')) {
        if ((pos = parameter.find("=")) != string::npos) {
            key = parameter.substr(0, pos);
            value = parameter.substr(pos+1, parameter.size());
            _queryParameters[key] = value;
        }
    }
}

void
http::Request::setReferer(string header)
{
    stringstream ss(header);
    string line;
    size_t found;

    while (std::getline(ss, line)) {
        string keyword = "Referer: ";
        if (line.find(keyword) != string::npos) {
            if ((found = line.find_last_of("/")) != string::npos)
                this->_referer = line.substr(found);
            else
                this->_referer = "/index.html";
            break ;
        }
    }
}

/* --------------------------------- Getters -------------------------------- */

string
http::Request::getPathToRequestedFile()
{
    string path;
    size_t found = 0;
    string storagePath = "/public/storage";
    
    // if ((found = _uri.find(storagePath)) != string::npos)
    //     _uri = _uri.substr(found+storagePath.size());

    if ((found = _uri.find("/public")) != string::npos)
        path = ".";
    else
        path = "./public";

    if (_method == "GET")
    {
        if (_uri == "/") {
            _uri = "/index.html";
            path += _uri;
        }
        else
            path += _uri;
    }
    else if (_method == "POST" || _method == "DELETE")
    {
        path += "/uploaded.html";
        _uri = path;
    }
    return path;
}

/////////////////////////////////////////////////////////////////////////////
/*                             EXCEPTIONS                                  */
/////////////////////////////////////////////////////////////////////////////
const char* http::AcceptFailed::what() const throw()
{
    return "accept() call failed";
}

const char* http::ConnectionClosed::what() const throw()
{
    return "";
}

const char* http::ReceiveFailed::what() const throw()
{
    return "Reading issue because of recv()";
}

const char* http::BadRequest::what() const throw()
{
    return "Bad request recieved by server";
}

const char* http::EmptyRequest::what() const throw()
{
    return "Empty request";
}