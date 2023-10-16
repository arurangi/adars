# About [in progress]
This project is about building an HTTP server from scratch (without libraries) in C/C++.

## Objectives
### Client (browser)
1. go to ``localhost:<port>/index.html`` and see the home page with its corresponding CSS

### Server
- listens for incoming TCP connections on chosen port
- processes request (HTTP)
- locate ressource
- build and send response

## Common Bugs:
- [Bind failed: Address already in use](https://stackoverflow.com/questions/15198834/bind-failed-address-already-in-use)

## Useful Links
- Important functions: [socket](https://pubs.opengroup.org/onlinepubs/9699919799/functions/socket.html), [bind](https://pubs.opengroup.org/onlinepubs/009695399/functions/bind.html), [connect](https://pubs.opengroup.org/onlinepubs/009695399/functions/connect.html), [listen](https://pubs.opengroup.org/onlinepubs/9699919799/functions/listen.html), [accept](https://pubs.opengroup.org/onlinepubs/9699919799.2013edition/functions/accept.html), [read](https://pubs.opengroup.org/onlinepubs/009604599/functions/read.html), [write](https://pubs.opengroup.org/onlinepubs/009696699/functions/write.html)