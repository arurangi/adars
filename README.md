# About [in progress]
This project is about building an HTTP server from scratch (without libraries) in C/C++.

<details>
<summary>Show Notes</summary>
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

- Sockets are the low level endpoint used for processing information accross a network
- Sockets are endpoints for communication.  
- A socket is identified by an IP address concatenated by a port number
- The server waits for incoming clients requests by listening to specified ports. Once a request is received, the server accepts a connection from the client socket to complete the connection.
- Servers implementing specific services (such as telnet, ftp, http) listen to well known ports. (a telnet server listens to port 23, an ftp server to port 21 and an http server to port 80).
- All ports below 1024 are considered well-known; we can use them to implement standard services
- common networking protocols like HTTP, or FTP rely on sockets underneath to make connections
- a socket is a two-way endpoint, you can both send and receive from a single socket. There is no such thing as just a client socket or a server socket

> IP:

Identifier for a computer or device on a network. Equivalent to its full name.

An IP determines the location of the server.

> Ports:

- A port number determines which service or program on that computer it wants to use. 

- port numbers use 2 bytes (16 bits). Meaning they can represent 65536 values.
- 3 catagories of port numbers:
| Port numbers | Description | Exemples |
| -- | -- | -- |
| 0 - 1023      | system or well-known ports| 80 & 443 (web pages), 21 (FTP), 25 (SMTP) |
| 1024 - 49 151 | user or registered ports. These ports can be registered by companies of developpers for a particular service. | 1102 (adobe), 1433 (Microsoft SQL server), 1416 (Novell), 1527 (Oracle)|
| 49 152 - 65 535   | dynamic or private ports. Client-side ports that are free to use. These are the ports that your computer assigns temporarily to itself during sessions. | while viewing a web page|

 The first two categories are used on a server. While the last one is for client, meaning our computer. Whenever our computer wants to use a service or program of another computer it assigns itself a port number of range category 3 to communicate.  

- a port number is always associated with an IP address 

> TCP/UDP: 

> HTTP: 

> Server: 

> Client: 

> HTTP Request: 

> HTTP: Response: 
</details>

## Communication Workflow
### Client (browser)
1. visite ``localhost:80/ressource_path``

### Server
0. parse configuration file
0. start TCP/IPV4 connection
2. listen for incoming connection + accept connections
4. process request
5. construct response + send
5. close the socket

## Use cases
- anything that interracts with a network can be built using the basic socket building blocks

## Ressources
- Important functions: [socket](https://pubs.opengroup.org/onlinepubs/9699919799/functions/socket.html), [bind](https://pubs.opengroup.org/onlinepubs/009695399/functions/bind.html), [connect](https://pubs.opengroup.org/onlinepubs/009695399/functions/connect.html), [listen](https://pubs.opengroup.org/onlinepubs/9699919799/functions/listen.html), [accept](https://pubs.opengroup.org/onlinepubs/9699919799.2013edition/functions/accept.html), [read](https://pubs.opengroup.org/onlinepubs/009604599/functions/read.html), [write](https://pubs.opengroup.org/onlinepubs/009696699/functions/write.html)