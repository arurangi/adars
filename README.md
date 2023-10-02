# About
This project is about building an HTTP server from scratch (without libraries) in C/C++.

# Terms
__The beginning of wisdom is the definition of terms -- Socrates__

> **ISO (Open System Interconnection)
model**

The Open Systems Interconnection model (OSI model) is a conceptual model that characterizes and standardizes the communication functions of a telecommunication or computing system without regard to its underlying internal structure and technology. 

Its goal is the interoperability of diverse communication systems with standard protocols. The model partitions a communication system into abstraction layers. The original version of the model defined seven layers.

__To implement HTTP, we only care about **4th Layer: Transport Layer**.__

> **Transport Layer**: 

The Transport layer is primarily responsible for ensuring that data is transferred from one point to another reliably and without errors. For example, the Transport layer makes sure data are sent and received in the correct sequence.

The Transport layer provides flow control and error handling, and participates in solving problems concerned with the transmission and reception of packets. Common examples of Transport layer protocols are Transmission Control Protocol (TCP), User Datagram Protocol (UDP) and Sequenced Packet Exchange (SPX).

Note from [RFC 2616](https://www.ietf.org/rfc/rfc2616.txt):
__ HTTP communication usually takes place over TCP/IP connections. The default port is TCP 80, but other ports can be used. This does not preclude HTTP from being implemented on top of any other protocol on the Internet, or on other networks. HTTP only presumes a reliable transport; any protocol that provides such guarantees can be used; the mapping of the HTTP/1.1 request and response structures onto the transport data units of the protocol in question is outside the scope of this specification. __

RFC
: A Request for Comments (RFC), in the context of [Internet governance](https://en.wikipedia.org/wiki/Internet_governance), is a type of publication from the [Internet Engineering Task Force](https://en.wikipedia.org/wiki/Internet_Engineering_Task_Force) (IETF) and the [Internet Society](https://en.wikipedia.org/wiki/Internet_Society)(ISOC), the principal technical development and standards-setting bodies for the [Internet](https://en.wikipedia.org/wiki/Internet).
An RFC is authored by engineers and computer scientists in the form of a memorandum describing methods, behaviors, research, or innovations applicable to the working of the Internet and Internet-connected systems. It is submitted either for peer review or to convey new concepts, information, or (occasionally) engineering humor. The IETF adopts some of the proposals published as RFCs as [Internet Standards](https://en.wikipedia.org/wiki/Internet_Standard). Request for Comments documents were invented by Steve Crocker in 1969 to help record unofficial notes on the development of ARPANET. RFCs have since become official documents of Internet [specifications](https://en.wikipedia.org/wiki/Specification_(technical_standard)), [communications protocols](https://en.wikipedia.org/wiki/Communication_protocol), procedures, and events.

__ To implement TCP, we have to learn TCP socket programming. __

> Socket:

A socket is the mechanism that most popular operating systems provide to give programs access to the network. It allows messages to be sent and received between applications (unrelated processes) on different networked machines.

> IP:

> TCP/UDP: 

> HTTP: 

> Server: 

> Client: 

> Sockets: 

> Ports:

> HTTP Request: 

> HTTP: Response: 


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

## Programming with TCP/IP sockets

1. Create the socket
2. Identify the socket
3. On the server, wait for an incoming connection
4. Send and receive messages
5. Close the socket

## Use cases
- anything that interracts with a network can be built using the basic socket building blocks