#ifndef HTTP_HPP
    #define HTTP_HPP

    #define BUFFER_SIZE 1024

    typedef struct {
        std::string version;
        int         code;
        std::string message;
    } status_line;

    namespace http {

        #include <string>

        class Response {
            public:
                status_line _statusLine;
                std::string _header;
                std::string _body;
                const void* _raw;
                int         _contentLength;

                Response() {}
        };

        class Request {
            public:
                std::string _method;
                std::string _path;
                std::string _version;
                char        _raw[BUFFER_SIZE];
                std::string _body;
        
                Request() {}
        };
    }

#endif
