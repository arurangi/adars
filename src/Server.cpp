// Server side C program to demonstrate Socket programming

#include "../includes/Server.hpp"

Server::Server()
: _host("127.0.0.1"), _port(8000), _root("./public"), _storage_dir("./public/storage/")
{}

Server::Server(int domain, int service, int protocol, int backlog)
: _host("127.0.0.1"), _port(8000), _root("./public"), _storage_dir("./public/storage/")
{
    this->setup(domain, service, protocol, backlog);
}

Server&
Server::operator= (Server& rhs)
{
    this->_socket = rhs._socket;
    this->_host = rhs._host;
    this->_port = rhs._port;
    this->_root = rhs._root;
    this->_storage_dir = rhs._storage_dir;
    this->_locations = rhs.get_locations();
    this->_error_pages = rhs._error_pages;
    this->_request_body_size_limit = rhs._request_body_size_limit;
    this->_index = rhs._index;
    this->_server_name = rhs._server_name;
    return *this;
}

Server::~Server() {}

//////////////////////////////////////////////////////////////////////////////////

void
Server::setup(int domain, int service, int protocol, int backlog)
{
    int status, on = 1;

    /*************************************************************/
    /* Create an AF_INET4 stream socket to receive incoming      */
    /* connections on                                            */
    /*************************************************************/
    _socket = socket(domain, service, protocol);
    if (_socket < 0)
        exit(Log::out("socket() failed"));
    
    /*************************************************************/
    /* Allow socket descriptor to be reuseable                   */
    /*************************************************************/
    status = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (status < 0) {
        close(_socket);
        exit(Log::out("setsockopt() failed"));
    }

    /*************************************************************/
    /* Set socket to be nonblocking. All of the sockets for      */
    /* the incoming connections will also be nonblocking since   */
    /* they will inherit that state from the listening socket.   */
    /*************************************************************/
    status = ioctl(_socket, FIONBIO, (char *)&on);
    if (status < 0) {
        close(_socket);
        exit(Log::out("ioctl() failed"));
    }
    
    /*************************************************************/
    /* Bind the socket                                           */
    /*************************************************************/
    std::memset(&_address, 0, sizeof(_address));
    _address.sin_family = domain;
    _address.sin_port = htons(_port);
    status = inet_pton(AF_INET, _host.c_str(), &_address.sin_addr); // TODO: replace IP with server IP
    if (status <= 0) {
        close(_socket);
        exit(Log::out("inet_pton() failed: invalid IP address"));
    }
    status = bind(_socket, (struct sockaddr*)&_address, sizeof(_address));
    if (status < 0) {
        close(_socket);
        Log::error("bind() failed: Can't assign requested address");
        exit(-1);
    }
    
    /*************************************************************/
    /* Set the listen back log                                   */
    /*************************************************************/
    status = listen(_socket, backlog);
    if (status < 0) {
        close(_socket);
        exit(Log::out("listen() failed"));
    }
    
    /*************************************************************/
    /* Link server_name to host (IP address) : local DNS lookup  */
    /*************************************************************/
    std::ofstream hostsfile("/etc/hosts", std::ofstream::app);
    if (hostsfile.is_open()) {
        hostsfile <<  _host + " " + _server_name << std::endl;
        hostsfile.close();
    } else {
        Log::out("Couldn't link server_name to host");
    }

    Log::listening(_host, _port, _server_name);
    // Log::mark(_root);
    // Log::ltree(_locations);
}

/////////////////////////////////////////////////////////////////////////////////////////

// Checks no errors happened
void Server::check(int status, string error_msg)
{
    if (status < 0)
    {
        std::cout << "Error: " << error_msg << std::endl;
        throw Server::ListeningProblem();
    }
}

/* --------------------------------- Getters -------------------------------- */

int Server::get_port()
{
    return _port;
}

LocationsList::iterator Server::lbegin()
{
    return _locations.begin();
}
LocationsList::iterator Server::lend()
{
    return _locations.end();
}

/* --------------------------------- Setters -------------------------------- */

void Server::set_location(LocationsList locations)
{
    _locations = locations;
}

void Server::set_root(string rootFolder)
{
    _root = rootFolder;
}

void
Server::set_request_body_size_limit(string size_limit)
{
    _request_body_size_limit = std::atoi(size_limit.c_str());
}

void
Server::set_default_index(string index_page)
{
    _index = index_page;
}

void
Server::set_port(string port)
{
    if (port.find(":") != std::string::npos)
        port = port.substr(port.find_last_of(":")+1, port.size());
    _port = std::atoi(port.c_str());
}

void
Server::set_host(string host)
{
    _host = host;
}

void
Server::set_default_error_page(vector<string> pages)
{
    Log::status(pages[0] + ": " + pages[1]);
    _error_pages[pages[0]] = pages[1];
}

void
Server::set_server_name(string server_name)
{
    _server_name = server_name;
}


/* -------------------------------- Exception ------------------------------- */

const char*
Server::SocketCreationProblem::what() const throw()
{
    return "server could not CREATE socket";
}

const char*
Server::SocketBindingProblem::what() const throw()
{
    return "server could not BIND socket";
}

const char*
Server::ListeningProblem::what() const throw()
{
    return "server had a problem while LISTENING";
}

const char*
Server::SetupIssue::what() const throw()
{
    return "couldn't setup the server";
}

const char*
Server::RequestProcessingIssue::what() const throw()
{
    return "request processing issue";
}