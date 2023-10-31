#include "../includes/Cluster.hpp"

Cluster::Cluster() : _size(0) {}
Cluster::~Cluster()
{
    for (IteratorS it = this->begin(); it != this->end(); it++) {
        close((*it).second._socket);
    }
}

int get_port(std::vector<std::string> v)
{
    std::string s = v[0];
    s = s.substr(s.find_last_of(":")+1, s.size());
    return std::atoi(s.c_str());
}

std::string get_host(std::vector<std::string> v)
{
    std::string s = v[0];
    s = s.substr(0, s.find_first_of(":"));
    return s;
}

void
Cluster::init(Serv_list serverList)
{
    _size = serverList.size();
    std::string tmp;

    for(Serv_list::iterator it = serverList.begin(); it != serverList.end() /* && !wbsv_data.error.length()*/ ; ++it)
    {
        int port = 0;
        std::string host = "";
        for (map_vector_it server_data_it = it->server_data.begin(); server_data_it != it->server_data.end() /*&& !wbsv_data.error.length()*/; ++server_data_it)
        {
            for (std::vector<std::string>::iterator value_it = server_data_it->second.begin(); value_it != server_data_it->second.end() /*&& !wbsv_data.error.length()*/; value_it++)
            {
                if (server_data_it->first == "listen") {
                    port = get_port(server_data_it->second);
                    host = get_host(server_data_it->second);
                }
                // else if (server_data_it->first == "client_max_body_size")
                //     //
                // else if (server_data_it->first == "root")
                //     //
                // else if (server_data_it->first == "index")
                //     //
           }
        }
        Server a(IPV4,TCP,DEFAULT,port, host, BACKLOG);
        _servers[a._socket] = a;
    }


    // Server b(IPV4,TCP,DEFAULT,8081,BACKLOG);
    // _servers[b._socket] = b;

    _begin = _servers.begin();
    _end = _servers.end();
}

void
Cluster::watch()
{
    int status, curr_fd, setsize = FD_SETSIZE;

    // initialize data structure to store all known sockets (server and client)
    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    for (IteratorS it = this->begin(); it != this->end(); it++)
        FD_SET((*it).second._socket, &master_set);
    
    do
    {
        working_set = master_set; // because select() is destructive
        // wait for something to read
        status = select(setsize, &working_set, NULL, NULL, NULL);
        if (status < 0) {
            Log::out("select() call failed");
            break ;
        } else if (status == 0) {
            Log::error("select() timed-out. Ending program.");
            break ;
        }
        
        // identify which socket is readable
        for (curr_fd = 0; curr_fd < setsize; curr_fd++) {
            if (FD_ISSET(curr_fd, &working_set)) {
                // server = new TCP connection
                if (this->find(curr_fd)) {
                    Log::mark("Listening socket is readable");
                    try { /* accept() */
                        int new_client = http::accept_connection(curr_fd);
                        FD_SET(new_client, &master_set);
                    } catch (std::exception& e) {
                        Log::warn(e.what());
                    }
                }
                // client = new HTTP request
                else {
                    Log::mark("Descriptor " + ft::to_str(curr_fd) + " is readable");
                    try { /* recv() & write() */
                        http::handle_request(curr_fd);
                    } catch(std::exception& e) {
                        Log::warn(e.what());
                        close(curr_fd);
                    }
                    FD_CLR(curr_fd, &master_set);
                }
            }
        }
    }
    while (isRunning);

    // clean up all opened sockets
    for (curr_fd = 0; curr_fd <= setsize; ++curr_fd) {
        if (FD_ISSET(curr_fd, &master_set))
            close(curr_fd);
    }
}

std::map<int, Server>::iterator
Cluster::begin() { return _begin; }

std::map<int, Server>::iterator
Cluster::end() { return _end; }

int Cluster::size() { return _size; }

bool Cluster::find(int curr_fd)
{
    if (_servers.find(curr_fd) != _servers.end())
        return true;
        // return (*(_servers.find(curr_fd))).first;
    return false;
}