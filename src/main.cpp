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
    Log::success("CONFIGURATION FILE PARSING DONE");

    cluster.init(webservData.server_list);
    // exit(0);

    cluster.watch();

    return EXIT_SUCCESS;
}
