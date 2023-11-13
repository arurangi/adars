# About [in progress]
This project is about building an HTTP server from scratch (without libraries) in C/C++.

<img src="./.documentation/walkthrough_.gif" width="100%" />

## Objectives
### Client (browser)
1. go to ``localhost:<port>/<location>`` and see the home page with its corresponding CSS

### Features
- Can listen on multiple ports simultaneously
- Can accept multiple simultaneously connections
- NON-BLOCKING
- Handles GET, POST & DELETE requests
-- naviguate, send forms, upload and delete files
- Handles CGIs in python
- Uses an NGINX like configuration file

## Useful Links
**Core functions**
- [socket](https://pubs.opengroup.org/onlinepubs/9699919799/functions/socket.html), [bind](https://pubs.opengroup.org/onlinepubs/009695399/functions/bind.html), [connect](https://pubs.opengroup.org/onlinepubs/009695399/functions/connect.html), [listen](https://pubs.opengroup.org/onlinepubs/9699919799/functions/listen.html), [accept](https://pubs.opengroup.org/onlinepubs/9699919799.2013edition/functions/accept.html), [read](https://pubs.opengroup.org/onlinepubs/009604599/functions/read.html), [write](https://pubs.opengroup.org/onlinepubs/009696699/functions/write.html), [send](https://pubs.opengroup.org/onlinepubs/000095399/functions/send.html), [recv](https://pubs.opengroup.org/onlinepubs/007904975/functions/recv.html), [select](https://pubs.opengroup.org/onlinepubs/7908799/xsh/select.html#:~:text=The%20select()%20function%20tests,descriptors%20are%20ready%20to%20read.), [pol](https://pubs.opengroup.org/onlinepubs/000095399/functions/poll.html)
