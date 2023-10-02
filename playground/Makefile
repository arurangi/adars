COMPILER	:=	c++ -std=c++98
FLAGS		:=	-Wall -Werror -Wextra
SERVER_FILE	:=	server.cpp
CLIENT_FILE	:=	client.cpp

all:
	$(COMPILER) $(FLAGS) $(SERVER_FILE) -o server
	$(COMPILER) $(FLAGS) $(CLIENT_FILE) -o client


run_server:
	./server

run_client:
	./client

clean:
	rm server client
