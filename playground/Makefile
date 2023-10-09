COMPILER	:=	c++ -std=c++11
FLAGS		:=	-Wall -Werror -Wextra
SERVER_FILE	:=	server.cpp
CLIENT_FILE	:=	client.cpp

all:
	@$(COMPILER) $(FLAGS) $(SERVER_FILE) -o server
	@./server

run_server:
	./server

run_client:
	./client

clean:
	rm server client

re: clean all

.PHONY:	all run_client run_server clean re
