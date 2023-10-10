COMPILER	:=	c++ -std=c++98
FLAGS		:=	-Wall -Werror -Wextra
FILES	:=	server.cpp main.cpp client.cpp
CLIENT_FILE	:=	client.cpp
BIN			:=	webserv

all:
	@$(COMPILER) $(FLAGS) $(FILES) -o $(BIN)
	@./$(BIN)

run:
	./server

clean:
	rm -f $(BIN)

fclean: clean

re: clean all

.PHONY:	all run clean fclean re
