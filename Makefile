NAME = Codexion
CC = cc -Wall -Wextra -Werror -pthread
SRC = *.c
RM = rm -rf


all:
	$(CC) $(SRC) -o $(NAME)

clean:
	$(RM) $(NAME)

re:
	clean all