#ifndef SERVER_HPP
    #define SERVER_HPP

    #include <iostream>
    #include <string>
    #include <fstream>
    #include <map>
    #include <thread>
    #include <vector>

    #include <sys/socket.h>
    #include <unistd.h>
    #include <netinet/in.h>
    #include <sys/ioctl.h>
    #include <arpa/inet.h>

    #include "Http.hpp"
    #include "Utils.hpp"
    #include "Logger.hpp"
    #include "Client.hpp"

    using std::string;
    using std::vector;
    using std::map;
    // using http::Request;
    // using http::Response;

    typedef map< string, map<string, vector<string> > > LocationMap;
    typedef vector< LocationMap > LocationsList;

    #define IPV4                AF_INET
    #define IPV6                AF_INET6
    #define TCP                 SOCK_STREAM
    #define UDP                 SOCK_DGRAM
    #define DEFAULT             0
    #define BACKLOG_LISTENING   3
    #define BUFFER_SIZE         2047
    #define BYTES_PER_CHAR      2
    #define BLANK_LINE          "\r\n"
    #define PORT                8080
    #define BACKLOG             10

    enum states {
        isOff,
        isRunning
    };

    class Server {

        private:
            // ↓ needed for setup
            int                         _domain;
            int                         _service;
            int                         _protocol;
            int                         _backlog;
            struct sockaddr_in          _address;
            // ↓ needed while running
            string                      _host;
            int                         _port;
            string                      _root;
            string                      _storage_dir;
            LocationsList               _locations;
            int                         _request_body_size_limit;
            string                      _index;
            string                      _server_name;

        public:
            int     _socket;
            map<string, string > _error_pages;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // constructors
            Server();
            Server& operator= (Server& rhs);
            Server(int domain, int service, int protocol, int backlog);
            ~Server();

            ///////////////////////////////////////////////////////////////////////////////////////////
            // member functions

            void        setup(int domain, int service, int protocol, int backlog);
            static void check(int status, string error_msg);

            ///////////////////////////////////////////////////////////////////////////////////////////
            // setters

            void set_root(string rootFolder);
            void set_location(LocationsList locations);
            void set_request_body_size_limit(string size_limit);
            void set_default_index(string index_page);
            void set_port(string port);
            void set_host(string host);
            void set_server_name(string server_name);
            void set_default_error_page(vector<string> pages);

            int             get_port();
            string          get_server_name() const { return _server_name; }
            string          get_storage_dir() const { return _storage_dir; }
            LocationsList   get_locations() { return _locations; }
            string          get_root() { return _root; }

            LocationsList::iterator lbegin();
            LocationsList::iterator lend();


            ///////////////////////////////////////////////////////////////////////////////////////////
            // Exceptions
            class SocketCreationProblem : public std::exception {
                public:
                    const char* what() const throw();
            };
            class SocketBindingProblem : public std::exception {
                public:
                    const char* what() const throw();
            };
            class ListeningProblem : public std::exception {
                public:
                    const char* what() const throw();
            };
            class BadConnection : public std::exception {
                public:
                    const char* what() const throw();
            };
            class SetupIssue : public std::exception {
                public:
                    const char* what() const throw();
            };
            class RequestProcessingIssue : public std::exception {
                public:
                    const char* what() const throw();
            };

    };

#endif