// Server side C program to demonstrate Socket programming

#include "Server.hpp"
#include "Client.hpp"
#include "Http.hpp"
#include <pthread.h>
#include <sys/event.h> // kqueue

#define LOCALHOST "127.0.0.1"
#define PORT 8080
#define BACKLOG 10

#define ERROR_EXIT(...) { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(EXIT_FAILURE); }

class UserInterface {
    public:
        void status(std::string msg) {
            std::cout << std::endl << CYELLOW CBOLD
                      << "◌" 
                      << " " << msg << " " << CRESET << std::endl;
        }
        // void bsent(std::string msg, ) {
        //     std::cout << 
        // }
};
UserInterface ui;

void handleHttpRequest(int client_fd, std::map<string, string> mimeType)
{
    Request req = Server::processRequest(client_fd); // print
    Response res = Server::buildResponse(req, mimeType); // print

    int bytes_sent = 0;
    const char* tmp = (res._raw).c_str();

    size_t total = 0;

    std::cout << CGREEN << res._raw.length() << CRESET << std::endl; // GOOD
    std::cout << CGREEN << res._raw.size() << CRESET << std::endl; // GOOD
    std::cout << CRED << std::strlen(res._raw.c_str()) << CRESET << std::endl; // BAD, doesn't 
    std::cout << CRED << sizeof(res._raw.c_str()) << CRESET << std::endl;
    std::cout << CRED << sizeof(res._raw) << CRESET << std::endl;

    while (total < sizeof(tmp)) {
        bytes_sent = send(client_fd, tmp, res._raw.size(), 0); // res._contentLength
        if (bytes_sent < 0)
            std::cout << "in handleHttpRequest(): send(): No bytes to send";
        total += bytes_sent;
        std::cout << "< sending...\n";
    }
    std::cout << CGREEN << "••• Bytes transmitted: " << CBOLD << total << "/" << sizeof(tmp) << CRESET << std::endl;
            
    close(client_fd);
}


int main()
{
    Server server; Client client;
    struct kevent change_event[4], event[4];
    int socket_connection_fd;

    try {
        server.setup(IPV4, TCP, DEFAULT, PORT, BACKLOG);
        ui.status("Listening on 127.0.0.1:8080");

        // Create kqueue
        int kq = kqueue();

        // Create filter for events that we want to monitor on given fd
        // one the EVFILT_READ event has occured, we add it to the event queue.
        EV_SET(change_event, server._socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

        // Register events
        if (kevent(kq, change_event, 1, NULL, 0, NULL) == -1)
            ERROR_EXIT("kevent: register socket_listen_fd");
        
        ui.status("Event loop up and running!🚀");
        ui.status("Waiting for incoming events📡...");

        while (isRunning)
        {
            // Poll for new events
            int new_events = kevent(kq, NULL, 0, event, 1, NULL);
            if (new_events == -1)
                ERROR_EXIT("kevent: poll for events");
            
            for (int i = 0; i < new_events; i++)
            {
                int event_fd = event[i].ident;

                // Close fd when client disconnects
                if (event[i].flags & EV_EOF)
                    close(event_fd);
                // In case the events fd is the same as our listening fd
                // we have an incoming connection
                else if (event_fd == server._socket)
                {
                    printf("New connection coming in...\n");

                    socket_connection_fd = accept(event_fd, (struct sockaddr *)&client._address, (socklen_t *)&client._addrLength);
                    if (socket_connection_fd == -1)
                        ERROR_EXIT("accept socket");
                    
                    // Add the new connection to our interest list
                    EV_SET(change_event, socket_connection_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                    if (kevent(kq, change_event, 1, NULL, 0, NULL) < 0)
                        ERROR_EXIT("kevent: register socket_connection_fd");
                }
                else if (event[i].filter & EVFILT_READ)
                {
                    // Read from socket
                    // char buf[1024];
                    // size_t bytes_read = recv(event_fd, buf, sizeof(buf), 0);
                    // buf[bytes_read] = '\0'; 
                    std::map<string, string> mt = server._mimeTypes;
                    // recv(client_socket, req._raw, BUFFER_SIZE, 0)
                    handleHttpRequest(event_fd, mt);
                }
            }
        }
    }
    catch (std::exception& e) { // only catches initialization bugs
        std::cout << e.what();
    }
    close(server._socket);
    return EXIT_SUCCESS;
}