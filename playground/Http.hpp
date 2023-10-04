#ifndef HTTP_HPP
    #define HTTP_HPP

    #include <string>       // std::string
    #include <fstream>      // std::ifstream
    #include <iostream>     // std::getline()

    #include "Utils.hpp"

    #define CBLUE    "\033[0;94m"
    #define CYELLOW   "\033[0;33m"
    #define CMAGENTA  "\033[0;35m"
    #define CRESET    "\033[0m"

    #define BYTES_PER_CHAR 2

    class Http {

        __unused float       _version;

        __unused u_int16_t   _statusCode;
        std::string _statusMessage;
        std::string _contentType;
        long        _contentLength;

        public:
            Http() : _version(1.1) {}
            ~Http() {}

            // Process Request received
            void processRequest() {
                
                /* GET /info.html HTTP/1.1
                 * ---
                 * 1. File is present & client has permissions
                 * 2. Open file, read data into variable + count nbr bytes
                 * - update `Content-Type` and `Content-Length`
                */
                // if ( /* goes wrong */ )
                //     throw std::exception();
            }

            // construct the response header
            std::string respond(std::string content_type)
            {
                std::string     response, buffer, body, header;
                std::ifstream   requestedFile("index.html");

                if (!requestedFile.is_open())
                    exit(EXIT_FAILURE);
                
                while (std::getline(requestedFile, buffer))
                    body += buffer + "\n";
                _contentLength = body.size() * BYTES_PER_CHAR;
                requestedFile.close();

                header   = "HTTP/1.1 200 OK\n";
                header   += "Content-Type: " + content_type + "\n";
                header   += "Content-Length: " + Utils::to_str(_contentLength) + "\n";
            
                response = header + "\n" + body;

                std::cout << CBLUE << response << CRESET << std::endl;
                return response;
            }

            int getContentLength() { return _contentLength; }
    };

#endif
