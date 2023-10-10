#ifndef SERVER_HPP
    #define SERVER_HPP

    #include <iostream>
    #include <string>
    #include <fstream>
    #include <iostream>
    #include <map>

    #include <sys/socket.h>
    #include <unistd.h>
    #include <netinet/in.h>

    #include "Http.hpp"
    #include "Utils.hpp"

    #define IPV4                AF_INET
    #define IPV6                AF_INET6
    #define TCP                 SOCK_STREAM
    #define UDP                 SOCK_DGRAM
    #define DEFAULT             0
    #define BACKLOG_LISTENING   3
    #define BUFFER_SIZE         1024
    #define BYTES_PER_CHAR 2

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
            struct sockaddr_in _address;
            http::Request _request;
            std::map<std::string, std::string> _mimeTypes;

        public:

            int _socket;

            // constructors
            Server() {}
            ~Server() {}

            // member functions
            void init(int domain, int service, int protocol, int port, int backlog);
            void processRequest(const int& client_socket);
            http::Response buildResponse(const int& client_socket);

            static void check(int status, std::string error_msg);

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