#include "../includes/Cluster.hpp"

Cluster::Cluster() : _size(0)
{
    
}

Cluster::~Cluster()
{

}

void
Cluster::init()
{
    _size = 2;
    // _servers.push_back(Server(
    //     IPV4,
    //     TCP,
    //     DEFAULT,
    //     8080,
    //     BACKLOG
    // ));

    Server a(IPV4,TCP,DEFAULT,8080,BACKLOG);
    _servers[a._socket] = a;

    // _servers.push_back(Server(
    //     IPV4,
    //     TCP,
    //     DEFAULT,
    //     8081,
    //     BACKLOG
    // ));

    Server b(IPV4,TCP,DEFAULT,8081,BACKLOG);
    _servers[b._socket] = b;

    _begin = _servers.begin();
    _end = _servers.end();
}

std::map<int, Server>::iterator
Cluster::begin() { return _begin; }

std::map<int, Server>::iterator
Cluster::end() { return _end; }

int Cluster::size() { return _size; }

int Cluster::find(int socket)
{
    if (_servers.find(socket) != _servers.end())
        return (*(_servers.find(socket))).first;
    return -1;
}