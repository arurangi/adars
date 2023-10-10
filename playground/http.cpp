#include "http.hpp"

std::string
http::get_mime_type(std::string filepath, map<string, string> accepted_types)
{
    std::string defaultMime = "application/octet-stream";

    size_t dotPosition = filepath.find(".");
    if (dotPosition == string::npos) {
        std::cout << "No extension dotPosition\n";
        return defaultMime;
    }
    string extension = filepath.substr(dotPosition+1, filepath.size());

    map<string, string>::iterator it = accepted_types.begin();
    while (it != accepted_types.end())
    {
        if (it->first == extension)
            return it->second;
    }
    return defaultMime;
}

std::map<std::string, std::string>
http::store_mime_types() {
    // open mime file
    // read line by line
    // save in key value pairs in map
    // close mime file
    map<string, string> m;
    return m;
}

//////// OPERATOR OVERLOADING /////////////////////////////////

std::ostream&
operator<< (std::ostream& os, http::Request& rhs)
{
    std::string raw((char*)rhs._raw);
    std::string curr;
    std::stringstream ss(raw);

    os << "---\n| Request\n";
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

    os << "---\n| Response\n";
    while (std::getline(ss, currentLine)) {
        os << CGREEN << "< " << CRESET << currentLine << std::endl;
    }
    return os;
}

void
http::Response::set_status(int code, std::string msg) {
    _code = code;
    _message = msg;
}