#include "../includes/Http.hpp"

http::Response::Response() { reset(); }
http::Response::~Response() { reset(); }

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
http::Response::set_status(std::string code, std::string msg) {
    _code = code;
    _message = msg;
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
http::Request::setStatusLine(std::string& request)
{
    std::stringstream ss(request);
    std::string method, path, version;
    
    ss >> method >> path >> version;

    this->_method = method;
    this->_uri = path;
    this->_httpVersion = version;
}

void
http::Request::setContentLength(std::string& request)
{
    std::stringstream ss(request);
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
    std::string path = "./public";
    size_t found = 0;
    std::string storagePath = "/public/storage";

    if (_method == "GET")
    {
        if (_uri == "/") {// or other locations
            _uri = "/index.html";
            path += "/index.html";
        }
        else {
            if ((found = _uri.find(storagePath)) != std::string::npos)
                _uri = _uri.substr(found+storagePath.size());
            path += _uri;
        }
    }
    else if (_method == "POST")
    {
        // the page the request was made from
        path += _referer;
        _uri = (_referer == "/") ? "./index.html" : _referer;
        Log::status("Referer : " + _referer);
    }
    return path;
}