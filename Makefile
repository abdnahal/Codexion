NAME = Codexion
CC = cc -Wall -Wextra -Werror
SRC = *.c

all:
	$(CC) $(SRC) -o $(NAME)

clean:
	$(Rm)