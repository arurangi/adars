#ifndef HTTP_HPP
    #define HTTP_HPP

    #define BUFFER_SIZE 2047

    #include <string>
    #include <iostream>
    #include <sstream>
    #include <fstream>
    #include <map>
    #include <set>
    #include <algorithm>
    #include <limits>

    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <signal.h>
    #include <sys/wait.h>

    #include "Logger.hpp"
    #include "Utils.hpp"
    #include "Server.hpp"
    #include "Cluster.hpp"
    #include "Html.hpp"
    #include "Checks.hpp"

    using std::set;
    using std::string;
    using std::map;

    #define ON 1
    #define OFF 0
    #define TRUE 1
    #define FALSE 0
    #define AUTOINDEX ON

    class Client;
    class Server;
    class Cluster;

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

    #define DEFAULT_INDEX "index.html"

    #define QUERY_SPACE "%20"

    using std::string;
    using std::map;
    using std::ostream;
    using std::stringstream;
    // using namespace http;

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
        };

        class Request {
            public:
                string  _method;
                string  _uri;
                string  _httpVersion;

                string  _startline;
                string  _header;
                string _body;
                map<string, string> _postData;
                string _rawBody;

                map<string, string> _formData;

                string    _autoindex;

                string  _contentType;
                int     _contentLength;
                string  _contentLengthStr;
                char    _raw[BUFFER_SIZE];
                string  _referer;
                int     _server_port;
                map<string, string> _queryParameters;
                string              _querystr;

                string  _filename;
                string  _payload;

                string  _status;

                string _formBoundary;
                string _formEnd;

                string                      _cgiContent;
                std::map<string, string>	_env;
                std::string					_cgi_path;
                char						**_ch_env;
                char 						**_argv;
                pid_t						_cgi_pid;
                int							_exit_status;

                int	pipe_in[2];
		        int	pipe_out[2];

                Request();
                ~Request();

                void    setStatusLine(string& header);
                void    setContentLength(string& header);

                void    parse_header();
                void    parse_query();

                void    setReferer(string header);
                void    setFilename(string& body);
                void    setPayload(string& body);

                void    execute(Request& req, string storageDir);
                void    save_payload(string storageDir);

                void parseUnstructuredBody();
                void parseStructuredBody();
                bool isMultipartFormData();

                string  getPathToRequestedFile();
                
                void    handle_cgi(Request &req);
                void    initEnv(Request &req);
                string	execute(Request &req);
                void	clear();
                void    freeCgiEnv();

        };

        int         accept_connection(int serverSocket);
        void        handle_request(int client_socket, Cluster& servers);
        Request     parse_request(const int& client_socket);
        Response    build_response(Request& req, Server& server);
        void        send_response(int client_socket, Response& res);

        bool        check_cgi(Request & req);

        map<string, string> load_mimeTypes(std::string mimesFilePath);
        string              get_mimeType(string filepath, map<string, string> accepted_types);
        string              generate_directoryPage(string uri);
        std::string         generate_errorPage(string path);
        std::string         generate_storageList();

        
        class AcceptFailed : public std::exception {
            public:
                const char* what() const throw();
        };
        class Abort : public std::exception {
            public:
                const char* what() const throw();
        };
        class ConnectionClosed : public std::exception {
            public:
                const char* what() const throw();
        };

        class BadRequest : public std::exception {
            public:
                const char* what() const throw();
        };

        class EmptyRequest : public std::exception {
            public:
                const char* what() const throw();
        };


        /////////////////////////////////////////////////////////////////////////////////////////
    }

    ostream& operator<< (ostream& os, http::Request& rhs);
    ostream& operator<< (ostream& os, http::Response& rhs);

#endif
