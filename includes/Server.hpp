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

    #include "Http.hpp"
    #include "Utils.hpp"
    #include "Logger.hpp"
    #include "Client.hpp"

    using std::string;
    using http::Request;
    using http::Response;

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
            int _domain;
            int _service;
            int _protocol;
            int _backlog;
            struct sockaddr_in _address;
            std::string _storageDir;
            std::vector<std::string> _allowedPaths;

        public:
            int _socket;
            uint32_t _host;
            int _port;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // constructors
            Server();
            Server(int domain, int service, int protocol, int port, int backlog);
            ~Server();

            ///////////////////////////////////////////////////////////////////////////////////////////
            // member functions

            void setup(int domain, int service, int protocol, int port, int backlog);
            static void check(int status, string error_msg);

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