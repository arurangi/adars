#ifndef CLUSTER_HPP
    #define CLUSTER_HPP

    #include <iostream>
    #include <vector>
    #include <sys/socket.h>
    #include "Server.hpp"
    #include "Webserv.hpp"

    class Server;

    typedef std::map<int, Server>::iterator IteratorS;
    using std::map;

    class Cluster {
        private:
            map<int, Server>  _servers;
            int               _size;
            IteratorS         _begin;
            IteratorS         _end;
        public:
            Cluster();
            ~Cluster();
            void        init(Serv_list serverList);
            void        watch();
            int         size();
            bool        find(int socket);
            IteratorS   begin();
            IteratorS   end();
            Server      getServerByPort(int port);
    };

    std::ostream& operator<< (std::ostream& os, Cluster& rhs);

#endif