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

    #include "http.hpp"
    #include "Utils.hpp"

    using std::string;
    using http::Response;
    using http::Request;
    using std::map;

    #define IPV4                AF_INET
    #define IPV6                AF_INET6
    #define TCP                 SOCK_STREAM
    #define UDP                 SOCK_DGRAM
    #define DEFAULT             0
    #define BACKLOG_LISTENING   3
    #define BUFFER_SIZE         2047
    #define BYTES_PER_CHAR 2
    #define BLANK_LINE "\n"

    enum states {
        isOff,
        isRunning
    };

    class Server {

        private:
            int _domain;
            int _service;
            int _protocol;
            int _port;
            int _backlog;
            struct sockaddr_in  _address;
            Request             _request;

        public:

            int _socket;

            int                 _cSocket;
            struct sockaddr_in  _cAddr;
            socklen_t  _cAddrLength;

            map<string, string> _mimeTypes;

            // constructors
            Server();
            ~Server();

            // member functions
            void            init(int domain, int service, int protocol, int port, int backlog);
            static Request  processRequest(const int& client_socket);
            static Response buildResponse(Request req, std::map<string, string> mimeType);

            static void check(int status, string error_msg);

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
    };

#endif