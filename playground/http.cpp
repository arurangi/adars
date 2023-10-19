#include "Http.hpp"

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

std::map<std::string, std::string>
http::store_mime_types() {
    std::map<std::string, std::string> tmp;

    // open mime file
    std::ifstream mimeFile("./conf/mime.types");
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

std::string http::get_gmt_time()
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

//////// OPERATOR OVERLOADING /////////////////////////////////

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

void
http::Response::set_status(std::string code, std::string msg) {
    _code = code;
    _message = msg;
}
