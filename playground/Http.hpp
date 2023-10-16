#ifndef HTTP_HPP
    #define HTTP_HPP

    #define BUFFER_SIZE 2047

    #include <string>
    #include <iostream>
    #include <sstream>
    #include <fstream>
    // #include <thread>
    #include <pthread.h>
    #include <thread> // TODO: delete this. Not in c++98
    #include <map>

    #define CBLUE    "\033[0;94m"
    #define CYELLOW   "\033[0;33m"
    #define CGREEN   "\033[0;32m"
    #define CMAGENTA  "\033[0;35m"
    #define CRED      "\033[0;31m"
    #define CRESET    "\033[0m"
    #define CBOLD   "\x1b[1m"

    #define BKBLUE  "\x1B[34m"

    using std::string;
    using std::map;

    typedef struct {
        std::string http_version;
        std::string code;
        std::string message;
    } status_line;

    namespace http {

        class Response {
            public:
                std::string _statusLine;
                std::string _httpVersion;
                std::string _code;
                std::string _message;

                std::string _header;
                std::string _contentType;
                int         _contentLength;

                std::string _body;

                std::string _raw;

                Response() {}

                void set_status(std::string code, std::string msg);
        };

        class Request {
            public:
                string _method;
                string _path;
                string _version;
                char        _raw[BUFFER_SIZE];
                string _body;
        
                Request() {}
        };

        /////////////////////////////////////////////////////////////////////////////////////////

        std::string          get_mime_type(std::string filepath, map<string, string> accepted_types);
        std::map<std::string, std::string>  store_mime_types();
    }

    std::ostream& operator<< (std::ostream& os, http::Request& rhs);
    std::ostream& operator<< (std::ostream& os, http::Response& rhs);

#endif
