# About
This project is about building an HTTP server from scratch (without libraries) in C/C++.

# Terms
> The beginning of wisdom is the definition of terms -- Socrates

- HTTP: 
- Server: 
- Client: 
- Sockets: 
- TCP/UDP: 
- IP:
- Ports:
- HTTP Request: 
- HTTP: Response: 

## Workflow

### The server
0. Read & Parse configuration file
1. Set Up a TCP Socket
2. Accept Incoming Connections
3. Read & Parse HTTP Requests
4. Process request, generate & send responses

### The client 
1. Set Up a TCP Socket
2. Connect to remote server
3. Send HTTP request
4. Display HTTP reponse

## sockets

- Sockets are the low level endpoint used for processing information accross a network
- Sockets are endpoints for communication.  
- A socket is identified by an IP address concatenated by a port number
- The server waits for incoming clients requests by listening to specified ports. Once a request is received, the server accepts a connection from the client socket to complete the connection.
- Servers implementing specific services (such as telnet, ftp, http) listen to well known ports. (a telnet server listens to port 23, an ftp server to port 21 and an http server to port 80).
- All ports below 1024 are considered well-known; we can use them to implement standard services
- common networking protocols like HTTP, or FTP rely on sockets underneath to make connections
- a socket is a two-way endpoint, you can both send and receive from a single socket. There is no such thing as just a client socket or a server socket

### Client socket workflow
1. Create a socket: needs type of socket, domain and protocol.
2. Connect it to a remote addres : needs IP & port
3. If successful connection, can start sending and receiving data

+----------+
| socket() |
+----------+
      |
      |
      V
+-----------+
| connect() |
+-----------+
      |
      |
      V
+-----------+
|   recv()  |
+-----------+

## Use cases
- anything that interracts with a network can be built using the basic socket building blocks