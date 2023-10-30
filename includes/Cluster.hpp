#ifndef CLUSTER_HPP
    #define CLUSTER_HPP

    #include <iostream>
    #include <vector>
    #include "Server.hpp"
    #include "Webserv.hpp"

    typedef std::map<int, Server>::iterator IteratorS;

    class Cluster {
        private:
            int _size;
            // std::vector<Server> _servers;
            std::map<int, Server> _servers;
            IteratorS _begin;
            IteratorS _end;
        public:
        Cluster();
        ~Cluster();
            void init(Serv_list serverList);
            int size();
            int find(int socket);
            IteratorS begin();
            IteratorS end();
    };

    std::ostream& operator<< (std::ostream& os, Cluster& rhs);

#endif