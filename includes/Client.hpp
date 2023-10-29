#ifndef CLIENT_HPP
    #define CLIENT_HPP

    #include <sys/socket.h>
    #include <netinet/in.h>

    class Client {
        private:
            // 
        public:
            int                 _socket;
            struct sockaddr_in  _address;
            socklen_t           _addrLength;

            Client() : _addrLength(sizeof(_address)) {}
            Client(int socket) : _socket(socket), _addrLength(sizeof(_address)) {}
            ~Client() {}
    };

#endif 