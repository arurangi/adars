#ifndef SERVER_HPP
    #define SERVER_HPP

    #include <iostream>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <netinet/in.h>
    #include <string>
    #include <fstream>
    #include <iostream>
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
            Http::Request _request;

        public:

            int _socket;

            // constructors
            Server() {}
            ~Server() {}

            // member functions
            void create(int domain, int service, int protocol, int port);
            void listen(int backlog);
            void processRequest(const int& client_socket);
            void sendResponse(const int& client_socket);
            std::string respond(std::string file);
            int getContentLength();
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
    };

#endif