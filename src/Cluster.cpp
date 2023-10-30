#include "../includes/Cluster.hpp"

Cluster::Cluster() : _size(0)
{
    
}

Cluster::~Cluster()
{

}

void
Cluster::init(Serv_list serverList)
{
    _size = serverList.size();
    // _servers.push_back(Server(
    //     IPV4,
    //     TCP,
    //     DEFAULT,
    //     8080,
    //     BACKLOG
    // ));

    for(Serv_list::iterator it = serverList.begin(); it != serverList.end(); ++it)
    {
        
    }

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

int Cluster::find(int curr_fd)
{
    if (_servers.find(curr_fd) != _servers.end())
        return (*(_servers.find(curr_fd))).first;
    return -1;
}