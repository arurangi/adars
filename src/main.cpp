// The brain of the HTTP server

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Http.hpp"
#include "../includes/Cluster.hpp"
#include "../includes/Webserv.hpp"

int main(int ac, char **av)
{
    Cluster cluster;
    Data    data;

    try
    {
        switch (ac)
        {
            case 1:
                data.config.parsing_file("conf/default.config", data);
                break ;
            case 2:
                data.config.parsing_file(av[1], data);
                break ;
            default:
                exit(Log::out("Wrong numers of arguments"));
        }
        if (data.error.length())
            exit(Log::out(data.error));      
        cluster.init(data.server_list);
    }
    catch (std::exception& e) {
        exit(Log::out(e.what()));
    }
    signal(SIGINT, ft::sighandler);
    cluster.watch();
    return EXIT_SUCCESS;
}
