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

std::string
http::get_mime_type(std::string filepath, map<string, string> accepted_types)
{
    std::string defaultMime = "application/octet-stream";

    size_t dotPosition = filepath.find_last_of(".");
    if (dotPosition == string::npos) {
        Log::error("Can't identify MIME type: No extension.");
        return defaultMime;
    }
    string extension = filepath.substr(dotPosition);
    Log::status("extension:" + extension);

    if (accepted_types.find(extension) != accepted_types.end())
        return accepted_types[extension];
    Log::error("Can't find (" + extension + ") in mime types");
    return "text/html";
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