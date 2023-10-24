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

    #include "Logger.hpp"

    class Client;

    #define CBLUE    "\033[0;94m"
    #define CYELLOW   "\033[0;33m"
    #define CGREEN   "\033[0;32m"
    #define CMAGENTA  "\033[0;35m"
    #define CRED      "\033[0;31m"
    #define CB_YELLOW      "\033[0;43m"
    #define CRESET    "\033[0m"
    #define CBOLD   "\x1b[1m"

    #define BKBLUE  "\x1B[34m"

    #define CRLFCRLF "\r\n\r\n"
    #define LF "\n"
    #define CRLF_SIZE 4
    #define END_OF_HEADER CRLFCRLF

    using std::string;
    using std::map;

    namespace http {

        class Response {
            public:
                std::string _httpVersion;
                std::string _code;
                std::string _message;

                std::string _statusLine;
                std::string _header;
                std::string _body;

                std::string _contentType;
                size_t      _contentLength;

                std::string _raw;

                Response();
                ~Response();

                void        set_status(std::string code, std::string msg);
                std::string get_gmt_time();
                void        reset();
        };

        class Request {
            public:
                string _method;
                string _uri;
                string _httpVersion;

                std::string _startline;
                char _header[BUFFER_SIZE];
                char _body[BUFFER_SIZE];

                int _contentLength;
                char _raw[BUFFER_SIZE];

                std::string _filename;
                std::string _payload;
        
                Request();
                ~Request();

                void setStatusLine(std::string& request);
                void setContentLength(std::string& request);
                void setFilename(string& body);
                void setPayload(string& body);
                std::string getPathToRequestedFile();
        };
        std::string get_mime_type(std::string filepath, map<string, string> accepted_types);

        /////////////////////////////////////////////////////////////////////////////////////////
    }

    std::ostream& operator<< (std::ostream& os, http::Request& rhs);
    std::ostream& operator<< (std::ostream& os, http::Response& rhs);

#endif
