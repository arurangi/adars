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
    #include <sys/socket.h>
    #include <netinet/in.h>

    #include "Logger.hpp"
    #include "Utils.hpp"

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

    #define HTTP_OK "200"
    #define HTTP_CREATED "201"
    #define HTTP_NO_CONTENT "204"
    #define HTTP_NOT_FOUND "404"
    #define HTTP_FORBIDDEN "403"
    #define HTTP_BAD_REQUEST "400"
    #define HTTP_METHOD_NOT_ALLOWED "405"
    #define HTTP_NOT_MODIFIED "304"
    #define HTTP_INTERNAL_SERVER_ERROR "500"


    using std::string;
    using std::map;
    using std::ostream;
    using namespace http;

    namespace http {

        class Response {
            public:
                string _httpVersion;
                string _code;
                string _message;

                map<string, string> _statusCodes;

                string  _statusLine;
                string  _header;
                string  _body;

                string  _contentType;
                size_t  _contentLength;
                string  _raw;

                Response();
                ~Response();

                void    set_status(string code);
                string  get_gmt_time();
                void    reset();
                string set_content_type(string filepath, map<string, string> accepted_types);
        };

        class Request {
            public:
                string _method;
                string _uri;
                string _httpVersion;

                string _startline;
                char _header[BUFFER_SIZE];
                char _body[BUFFER_SIZE];

                int _contentLength;
                char _raw[BUFFER_SIZE];
                string _referer;

                string _filename;
                string _payload;

                string _status;
        
                Request();
                ~Request();

                void setStatusLine(string& header);
                void setContentLength(string& header);
                void setReferer(string header);
                void setFilename(string& body);
                void setPayload(string& body);
                string getPathToRequestedFile();
        };

        int accept_connection(int serverSocket);

        void handle_request(int client_socket);
        Request process_request(const int& client_socket);
        Response build_response(Request& req, map<string, string>& mimeType);
        void send_response(Client& c, Response& res);
        void save_payload(Request& req)
        /////////////////////////////////////////////////////////////////////////////////////////
    }

    ostream& operator<< (ostream& os, Request& rhs);
    ostream& operator<< (ostream& os, Response& rhs);

#endif
