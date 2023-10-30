// The brain of the HTTP server

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Http.hpp"
#include "../includes/Cluster.hpp"
#include "../includes/Webserv.hpp"

int main(int ac, char **av)
{
    Cluster cluster;
    Data webservData;

    if (ac != 2)
        webservData.config.parsing_file("conf/config", webservData);
    else if (ac == 2)
        webservData.config.parsing_file(av[1], webservData);
    if (webservData.error.length())
    {
        std::cout << RED << webservData.error << END_CLR << std::endl;
        return 1;
    }
    std::cout << webservData.server_list.size() << std::endl;
    std::cout << "CONFIGURATION FILE PARSING DONE" << std::endl;

    try {
        cluster.init(webservData.server_list);
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }

    fd_set current_sockets, ready_sockets;
    FD_ZERO(&current_sockets);
    for (IteratorS it = cluster.begin(); it != cluster.end(); it++)
        FD_SET((*it).second._socket, &current_sockets);
    while (isRunning)
    {
        // because select is destructive
        ready_sockets = current_sockets;
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
            exit(Log::out("select error"));

        for (int curr_fd = 0; curr_fd < FD_SETSIZE; curr_fd++) {
            if (FD_ISSET(curr_fd, &ready_sockets)) {
                int found = cluster.find(curr_fd);
                if (found != -1) {
                    int new_clientSocket = http::accept_connection(found);
                    FD_SET(new_clientSocket, &current_sockets);
                } else {
                    http::handle_request(curr_fd);
                    FD_CLR(curr_fd, &current_sockets);
                }
            }
        }
    }
    for (IteratorS it = cluster.begin(); it != cluster.end(); it++)
        close((*it).second._socket);
    return EXIT_SUCCESS;
}
