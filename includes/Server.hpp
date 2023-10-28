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
            map<string, string> _mimeTypes;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // constructors
            Server();
            ~Server();

            ///////////////////////////////////////////////////////////////////////////////////////////
            // member functions

            void setup(int domain, int service, int protocol, int port, int backlog);
            int get_client();
            void handle_request(Client& c, Server& s);
            http::Request process_request(const int& client_socket);
            http::Response build_response(http::Request& req, std::map<string, string>& mimeType);
            void send_response(Client& c, http::Response& res);

            void save_payload(Request& req);

            std::map<std::string, std::string>  store_mime_types(std::string mimesFilePath);
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