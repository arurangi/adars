#ifndef HTTP_HPP
    #define HTTP_HPP

    #include <string>       // std::string
    #include <fstream>      // std::ifstream
    #include <iostream>     // std::getline()

    #include "Utils.hpp"

    #define CBLUE    "\033[0;94m"
    #define CYELLOW   "\033[0;33m"
    #define CMAGENTA  "\033[0;35m"
    #define CRED      "\033[0;31m"
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
                // if ( /* goes wrong */ )
                //     throw std::exception();
            }

            // construct the response header
            std::string respond(std::string file)
            {
                std::string     response, buffer, body, header;
                std::ifstream   requestedFile("." + file);
                
                if (!requestedFile.is_open())
                    exit(EXIT_FAILURE);

                // get content type
                size_t found = file.find(".");
                if (found == std::string::npos)
                    std::cout << "No extension found\n";
                std::string extension = file.substr(found+1, file.size());
                std::string _contentType = "text/" + extension;

                // store response body
                while (std::getline(requestedFile, buffer))
                    body += buffer + "\n";
                _contentLength = body.size() * BYTES_PER_CHAR;
                requestedFile.close();

                // construct HTTP response
                header   = "HTTP/1.1 200 OK\n";
                header   += "Content-Type: " + _contentType + "\n";
                header   += "Content-Length: " + Utils::to_str(_contentLength) + "\n";
                header   += "Connection: keep-alive\n"; // added for persistent connection
            
                response = header + "\n" + body;

                std::cout << CBLUE << response << CRESET << std::endl;
                return response;
            }

            int getContentLength() { return _contentLength; }
    };

#endif
