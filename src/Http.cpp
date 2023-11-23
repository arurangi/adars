#include "../includes/Http.hpp"

http::Response::Response() { reset(); }
http::Response::~Response() {}

http::Request::Request() : _contentLength(0), _referer(""){}
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
    ft::delay(1);
    if (client_socket < 0)
        throw http::AcceptFailed();
    return client_socket;
}

void
http::handle_request(int client_socket, Cluster& cluster)
{
    Request req = http::parse_request(client_socket);
    Server server = cluster.getServerByPort(req._server_port);
    req.execute(req, server.get_storage_dir());
    Response res = http::build_response(req, server);
    http::send_response(client_socket, res);
}

void
http::Request::execute(Request& req, string storageDir) {

    if (ft::startswith(req._uri, "/cgi-bin/") && ft::endswith(req._uri, ".py"))
    {
        Log::status("Execution: processing CGI");
        req._cgiContent = "";
        handle_cgi(req);
        size_t pos = req._cgiContent.find_last_of('>');
        req._cgiContent = req._cgiContent.substr(0, pos + 1);
        std::cout << req._cgiContent << std::endl;
    }
    else if (_method == "POST" && _contentLength > 0) {
        Log::status("Execution: saving payload to storage");
        save_payload(storageDir);
        _uri = "/storage";
    }
    else if (_method == "DELETE") {
        Log::status("Execution: deleting files from storage");
        string filename = _queryParameters["name"];
        string filepath = storageDir + filename;
        if (ft::startswith(filepath, "./public/")) {
            Log::status(">>>>>>>>> deleting file at: " + filepath + "<<<<<<<<<<<\n");
            remove(filepath.c_str());
        }
    }
}

void http::Request::handle_cgi(Request &req)
{
    clear();
    this->_cgi_path = req._uri;
    initEnv(req);
    req._cgiContent = execute(req);
}

void    http::Request::initEnv(Request &req)
{
    std::string cgi_exec = "." + req._uri;

    if (req._method == "POST")
    {
        std::stringstream out;
        out << req._body;
		this->_env["CONTENT_LENGTH"] = this->_contentLengthStr;
		this->_env["CONTENT_TYPE"] = this->_contentType;
    }
    this->_env["QUERY_STRING"] = req._querystr;
    this->_env["SCRIPT_NAME"] = "." + req._uri;
    this->_env["SCRIPT_FILENAME"] = this->_cgi_path;
    this->_env["PATH_INFO"] = this->_cgi_path;
    this->_env["PATH_TRANSLATED"] = this->_cgi_path;
    this->_env["REQUEST_URI"] = this->_cgi_path;
    this->_env["SERVER_NAME"] = "127.0.0.1";
    this->_env["SERVER_PORT"] = "8000";
    this->_env["REQUEST_METHOD"] = req._method;
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["REDIRECT_STATUS"] = "200";
    this->_env["SERVER_SOFTWARE"] = "ADARS";

    this->_ch_env = (char **)calloc(sizeof(char *), this->_env.size() + 1);
	std::map<std::string, std::string>::const_iterator it = this->_env.begin();
	for (int i = 0; it != this->_env.end(); it++, i++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->_ch_env[i] = strdup(tmp.c_str());
	}
	this->_argv = (char **)malloc(sizeof(char *) * 3);
	this->_argv[0] = strdup(cgi_exec.c_str());
	this->_argv[1] = strdup(this->_cgi_path.c_str());
	this->_argv[2] = NULL;
}

string    http::Request::execute(Request &req)
{
    std::string final;
    
    if (this->_argv[0] == NULL || this->_argv[1] == NULL)
	{
		return "CACA";
	}
	if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0)
	{
        Log::status("pipe() failed");
		return "CACA";
	}
	this->_cgi_pid = fork();
	if (this->_cgi_pid == 0)
	{
		dup2(pipe_in[0], 0);
		close(pipe_in[0]);
		close(pipe_in[1]);

        dup2(pipe_out[1], 1);
		close(pipe_out[0]);
		close(pipe_out[1]);
		this->_exit_status = execve(this->_argv[0], this->_argv, this->_ch_env);
		exit(this->_exit_status);
	}
	else if (this->_cgi_pid > 0){
        close(pipe_in[0]);
        close(pipe_out[1]);

        if (req._method == "POST")
        {
            write(pipe_in[1], this->_rawBody.c_str(), strlen(this->_rawBody.c_str()));
        }
        close(pipe_in[1]);


        char buffer[1024];
        ssize_t bytes;

        while ((bytes = read(pipe_out[0], buffer, sizeof(buffer))) > 0)
        {
            write(STDOUT_FILENO, buffer, bytes);
        }
        final = buffer;
    }
	else
	{
        std::cout << "Fork failed" << std::endl;
	}
    return final;
}

void		http::Request::clear()
{
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = "";
	this->_ch_env = NULL;
	this->_argv = NULL;
	this->_env.clear();
}

http::Request
http::parse_request(const int& client_socket)
{
    Request req;
    char    buffer[BUFFER_SIZE];
    string  raw_request;
    size_t  found;
    ssize_t bytesRead = std::numeric_limits<int>::max();

    /////// READ HEADER //////////


    do {
        bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesRead < 0)
            throw http::ReceiveFailed();
        raw_request += string(buffer, bytesRead);
    }
    while (bytesRead == BUFFER_SIZE);

    req._header = raw_request, req._body = raw_request;
    req.parse_header();

    // std::cout << req;

    if (req._method != "POST")
        return req;

    if ((found = req._body.find(CRLFCRLF)) != string::npos) {
        req._body = req._body.substr(found + CRLF_SIZE);

        if (req.isMultipartFormData()) {
            req.parseStructuredBody();
            req.setFilename(req._body);
            req.setPayload(req._body);
        }
        else
            req.parseUnstructuredBody();
    }
    return req;
}

void
http::Request::parseStructuredBody()
{
    stringstream    ss(_body);
    string          currLine;
    string          keyword = "Content-Disposition: form-data; name=\"";
    string key = "", value = "";
    size_t pos;

    while (std::getline(ss, currLine))
    {
        if (currLine.find(_formBoundary) != string::npos /*check ending*/) {
            if (!std::getline(ss, currLine))
                break ;
            if (ft::startswith(currLine, keyword)) {
                key = currLine.substr(keyword.size());
                key = key.substr(0, key.find_first_of("\"")); // remove " from ending

                std::getline(ss, currLine);
                if (ft::startswith(currLine, "Content-Type:"))
                    std::getline(ss, currLine);
                
                std::getline(ss, currLine);

                _postData[key] = currLine + LF;
            }
        }
        else if (!key.empty()) {
            if ((pos = currLine.find(_formBoundary+"--")) != string::npos) {
                Log::highlight("FOUND");
                currLine = currLine.substr(0, pos);
            }
            _postData[key] += currLine + LF;
        }
    }
    // std::cout << "__start_of_body +++\n";
    // std::cout << _postData["file"] << CYELLOW << "$" << CRESET;
    // std::cout << "__end_of_body +++\n";
}

void
http::Request::parseUnstructuredBody()
{
    stringstream ss(_body);
    string currLine;

    while (std::getline(ss, currLine))
        _rawBody += currLine;
}

bool
http::Request::isMultipartFormData()
{
    return (_contentType == "multipart/form-data");
}

string get_error_page(Server& s, string error_code)
{
    if (s._error_pages.find(error_code) != s._error_pages.end())
        return s._error_pages[error_code];
    else {
        if (ft::startswith(error_code, "4"))
            return "default400.html";
        else
            return "default500.html";
    }
}

http::Response
http::build_response(Request& req, Server& server)
{
    Response            res;
    string              buffer, path;
    std::ifstream       requestedFile;
    map<string, string> mimeTypes = http::load_mimeTypes("./conf/mime.types");
    string              error_page = get_error_page(server, "404");
    bool                body_is_set = false;

    // BODY SIZE TOO BIG -> sets path
    if (req._method == "POST" && req._contentLength > server.get_max_body_size()) {
        res.set_status("413");
        path = "./public/" + get_error_page(server, "413");
        // path = "./public/413.html";
    }
    /**************************************************************************/
    /* FIND RESSOURCE PATH: 3 options                                         */
    /* ***                                                                    */
    /* 1) CGI                                                                 */
    /* 2) filename                                                            */
    /* 3) location                                                            */
    /**************************************************************************/
    ////////////////////////////////////////////////////////////////////////////
    // DIRECTORY PATH -> sets body
    else if (ft::isdirectory(("."+req._uri).c_str())) {
        Log::status("===> directory");
        res._body = generate_directoryPage("./public/");
        res._contentLength = res._body.size();
        Log::mark(res._body);
        path = ".html";
        body_is_set = true;
    }
    ////////////////////////////////////////////////////////////////////////////
    // FILE PATH -> sets path
    else if  (get_mimeType(req._uri, mimeTypes) != "application/octet-stream") {
        Log::status("===> Filepath");
        path = req.getPathToRequestedFile();
    }
    ////////////////////////////////////////////////////////////////////////////
    // CGI -> sets body
    else if (ft::startswith(req._uri, "/cgi-bin")) {
        Log::status("===> CGI");
        if (req._cgiContent.empty()) {
            path = "./public/" + get_error_page(server, "501");
            req._uri = ".html";
        }
        else {
            res._body = req._cgiContent;
            res._body += '\0';
            res._contentLength = res._body.size();
            body_is_set = true;
            res._contentType = "text/html";
            path = ".html";
        }
        // req._uri = "./public/index.html";
        // path = "./public/" + error_page;
    }
    ////////////////////////////////////////////////////////////////////////////
    // LOCATIONS -> sets path
    else { // TODO: add condition, allowed locations (std::set)
        Log::status("===> Location");
        Log::mark("in location checking");
        bool found = false;
        string root = server.get_root();
        string index = "";
        LocationsList::iterator v = server.lbegin();
        for (; v != server.lend() && !found; v++) {
            LocationMap::iterator m = (*v).begin();
            for (; m != (*v).end() && !found; m++) {
                string location_path = (*m).first;
                if (req._uri == location_path)
                {
                    found = true; Log::success(req._uri);
                    
                    // default values
                    root    = server.get_root();
                    index   = DEFAULT_INDEX;

                    map<string, vector<string> > directives = (*m).second;
                    map<string, vector<string> >::iterator d = directives.begin();
                    for (; d != directives.end(); d++) {
                        string          type        = (*d).first;
                        vector<string>  settings    = (*d).second;
                        if (type == "return" && settings[0] == "302")
                        {
                            index = settings[1];
                            break ;
                        }
                        else if (type == "root")
                            root = settings[0];
                        else if (type == "index")
                            index = settings[0];
                        else if (type == "autoindex")
                            req._autoindex = settings[0];
                        else if (type == "allow_methods") {
                            vector<string>::iterator itr = std::find(settings.begin(), settings.end(), req._method);
                            if (itr == settings.end()) {  // not found
                                res.set_status("405");
                                error_page = get_error_page(server, "405");
                                index = error_page;
                                //break ;
                            }
                        } else if (type == "error_page") {
                            error_page = settings[1];
                        }
                    } /* End of loop through location settings */
                } /* End of 'if (req._uri == location_path)' */
            } /* End of loop through LocationMap (1) */
        } /* End of loop through LocationList */
        ////////////////////////////////////////////////////////////////////////////
        if (found) {
            path = root + "/" + index;
            std::cout << path << std::endl;
        } else {
            Log::error("Location not found");
            res.set_status("404"); // NOT FOUND
            error_page = get_error_page(server, "404");
            path = root + "/" + error_page;
        }
    } // end location

    /**************************************************************************/
    /* OPEN REQUESTED RESSOURCE (if body is not set)                          */
    /**************************************************************************/
    if (!body_is_set) {
        requestedFile.open(path.c_str(), std::ios::in);
        if (!requestedFile.is_open()) {
            res.set_status(HTTP_NOT_FOUND);
            if (req._autoindex == "on") {
                req._uri = ".html"; // TODO: can I delete this?
                res._body = http::generate_directoryPage(path);
            }
            else {
                req._uri = "/" + error_page; // TODO: can I delete this?
                res._body = generate_errorPage(error_page);
            }
        } else {
            while (std::getline(requestedFile, buffer)) {
                if (ft::endswith(path, "/uploaded.html") && buffer.find("</body>") != string::npos)
                    res._body += generate_storageList();
                res._body += buffer + "\n";
            }
            res._body += '\0';
            requestedFile.close();
        }
        res._contentLength = res._body.size();
    }

    ////////////////////////////////////////////////////
    // STATUS_LINE
    // : version, status_code, status_text
    res._statusLine = req._httpVersion + " " + res._code + " " + res._message + "\r\n";

    ////////////////////////////////////////////////////
    // HEADER:
    // : Content-Type, Content-Length, Connection
    res._header   += res._statusLine;
    res._header   += "Content-Type: " + get_mimeType(path, mimeTypes) + "\r\n";
    res._header   += "Content-Length: " + ft::to_str(res._contentLength) + "\r\n";
    res._header   += "Date: " + res.get_gmt_time() + "\r\n";
    res._header   += "Connection: keep-alive\r\n";
    res._header   += "Server: " + server.get_server_name() + "\r\n";
    res._header   += "Cache-Control: no-cache\r\n";
    res._header   += "\r\n";

    ////////////////////////////////////////////////////
    // FULL RESPONSE:
    // : header, body
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
    if (_method == "POST" && !_payload.empty()) {
        string path = storageDir + _filename; // TODO: prefix with _storage_dir
        Log::status("filepath: " + path);
        std::ofstream outputFile(path.c_str(), std::ios::binary);
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

/////////////////////////////////////////////////////////////////////////
string
http::get_mimeType(string filepath, map<string, string> accepted_types)
{
    string defaultMime = "application/octet-stream";

    size_t dotPosition = filepath.find_last_of(".");
    if (dotPosition == string::npos) {
        Log::error("Can't identify MIME type: No extension.");
        return defaultMime;
    }
    string extension = filepath.substr(dotPosition);

    if (accepted_types.find(extension) != accepted_types.end())
        return accepted_types[extension];
    return defaultMime; //"text/html";
}

std::map<string, string>
http::load_mimeTypes(string mimesFilePath) {
    std::map<string, string> tmp;
    string line, key, value;
    std::ifstream mimeFile;
    int seperator;

    // open mime file
    mimeFile.open(mimesFilePath.c_str(), std::ios::in);
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
                    "<li><a href=\"/\">Home</a></li>\n"
                    "<li><a href=\"/about\">About</a></li>\n"
                    "<li><a href=\"/send-files\">Send files</a></li>\n"
                    "<li><a href=\"/storage\">Storage</a></li>\n"
                    "<li><a href=\"/form\">Form</a></li>\n"
                    "</ul>\n"
                "</nav>"
                "<h1>Directory listing</h1>\n"
                "<div class=\"dirlist\">\n";
                
    while (!list.empty()) {
        htmlPage += "<p class=\"dirfiles\">" + list.front() + "</p>\n";
        list.pop_front();
    }
    htmlPage += "</div>\n"
                "</section>\n"
                "</body>\n"
                "</html>\n";
    
    return htmlPage;
}

string
http::generate_errorPage(string path)
{
    std::ifstream requestedFile(("./public/" + path).c_str(), std::ios::in);
    string body = "", buffer = "";
    while (std::getline(requestedFile, buffer)) {
        body += buffer + "\n";
    }
    body += '\0';
    requestedFile.close();

    return body;
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

    if (list.empty()) {
        storageItem += "<p>Nothing here</p>";
        return storageItem;
    }
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
    while (std::getline(ss, currentLine) && currentLine != CRLFCRLF)
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
        Log::error("setFilename(): No `Content-Disposition` in POST request");
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
        Log::error("setPayload(): No `Content-Disposition` in POST request");
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
        else {
            this->_payload += line + LF;
        }
    }
}

void
http::Request::parse_header()
{
    string          line;
    size_t          found;
    string          tmp = _header.substr(0, _header.find(CRLFCRLF));
    stringstream    ss(tmp);
    
    ss >> _method >> _uri >> _httpVersion;
    if (_method != "GET" && _method != "POST" && _method != "DELETE") { // special func
        if (_uri.empty() && _httpVersion.empty())
            throw EmptyRequest();
        _status = HTTP_BAD_REQUEST;
    }

    this->parse_query();

    while (std::getline(ss, line) && line != CRLFCRLF) {
        string keyword = "Content-Type: ";
        if (line.find(keyword) != string::npos) {
            line = line.substr(keyword.size(), line.size());
            if ((found = line.find(";")) != string::npos) {
                _contentType = line.substr(0, found) + "\0";
            }
            else
                _contentType = line;
            keyword = "boundary=";
            if ((found = line.find(keyword)) != string::npos) {
                _formBoundary = line.substr(found+keyword.size());
                trim(_formBoundary, WHITE_SPACE); // remove leading spaces
            }
        }
        keyword = "Content-Length: ";
        if (line.find(keyword) != string::npos) {
            string length = line.substr(keyword.size(), line.size());
            this->_contentLengthStr = length;
            this->_contentLength = std::atoi(length.c_str());
            Log::highlight(_contentLength);
        }
        keyword = "Host: ";
        if (line.find(keyword) != string::npos) {
            string host = line.substr(keyword.size(), line.size());
            string port = host.substr(host.find_last_of(":")+1);
            this->_server_port = std::atoi(port.c_str());
        }
        keyword = "Referer: ";
        if (line.find(keyword) != string::npos) {
            if ((found = line.find_last_of("/")) != string::npos)
                this->_referer = line.substr(found);
            else
                this->_referer = "/index.html";
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

    if (_uri.find_first_of("?") == string::npos)
        return ;
    pos = _uri.find_first_of("?");
    query = _uri.substr(
        pos+1,
        (_uri.find("#") != string::npos) ? _uri.find_first_of("#") : _uri.size()
    );
    _querystr = query;
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

/* --------------------------------- Getters -------------------------------- */

string
http::Request::getPathToRequestedFile()
{
    string path;
    size_t found = 0;
    string storagePath = "/public/storage";
    string style = "/stylesheets";

    if ((found = _uri.find(style)) != string::npos)
        _uri = _uri.substr(found);

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