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

## Bugs & Hints:
Here are a few bugs I had a hints on how to solve them.
- Bind failed: Address already in use. ([stackoverflow](https://stackoverflow.com/questions/15198834/bind-failed-address-already-in-use))
- ERR_CONTENT_LENGTH_MISMATCH => Content-Length = `body.size()` and `send()` needs `entierResponse.size()`
- Data loss in uploaded file => `2 recv, 1 w/loop` and `0A`

## Useful Links
**Core functions**
- [socket](https://pubs.opengroup.org/onlinepubs/9699919799/functions/socket.html), [bind](https://pubs.opengroup.org/onlinepubs/009695399/functions/bind.html), [connect](https://pubs.opengroup.org/onlinepubs/009695399/functions/connect.html), [listen](https://pubs.opengroup.org/onlinepubs/9699919799/functions/listen.html), [accept](https://pubs.opengroup.org/onlinepubs/9699919799.2013edition/functions/accept.html), [read](https://pubs.opengroup.org/onlinepubs/009604599/functions/read.html), [write](https://pubs.opengroup.org/onlinepubs/009696699/functions/write.html), [send](https://pubs.opengroup.org/onlinepubs/000095399/functions/send.html), [recv](https://pubs.opengroup.org/onlinepubs/007904975/functions/recv.html), [select](https://pubs.opengroup.org/onlinepubs/7908799/xsh/select.html#:~:text=The%20select()%20function%20tests,descriptors%20are%20ready%20to%20read.), [pol](https://pubs.opengroup.org/onlinepubs/000095399/functions/poll.html)

**Debugging**
- [Araxis Merge](https://www.araxis.com/merge/index.en): it's a binary file comparaison tool. This was useful when uploading files to the server via POST request. It allowed me to compare the data received vs the data in the original file. (used their 30day free trial)
