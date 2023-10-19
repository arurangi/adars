COMPILER	:=	c++ -std=c++11
FLAGS		:=	-Wall -Werror -Wextra
FILES		:=	http.cpp server.cpp main_kqueue.cpp Utils.cpp
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
