FROM alpine:3.14

RUN apk update
RUN apk add build-base
RUN apk add vim

WORKDIR /home

COPY . /adars

WORKDIR /adars

RUN make

EXPOSE 8000

CMD ["./webserv", "./conf/container.config"]