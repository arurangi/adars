#include "../includes/Http.hpp"

http::Response::Response() { reset(); }
http::Response::~Response() { reset(); }

http::Request::Request() { _contentLength = 0; }
http::Request::~Request() { _contentLength = 0; }

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

    size_t dotPosition = filepath.find(".");
    if (dotPosition == string::npos) {
        std::cout << "No extension dotPosition\n";
        return defaultMime;
    }
    string extension = filepath.substr(dotPosition, filepath.size());

    map<string, string>::iterator it = accepted_types.begin();
    while (it != accepted_types.end())
    {
        if (it->first == extension) {
            // std::cout << "Type = " << CGREEN << it->second << CRESET << std::endl;
            return it->second;
        }
        it++;
    }
    return defaultMime;
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
    std::string content(rhs._raw);
    std::string currentLine;
    std::stringstream ss(content);

    std::thread::id threadId = std::this_thread::get_id();
    os << CGREEN CBOLD << "\n---\n| Response " << CRESET
       << CYELLOW << threadId << CRESET << std::endl;
    
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
            this->_payload += line;
        }
        else
            this->_payload += line;
    }
}

void
http::Request::setStatusLine(std::string& request)
{
    std::stringstream ss(request);
    std::string method, path, version;
    
    ss >> method >> path >> version;

    this->_method = method;
    this->_path = path;
    this->_version = version;
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