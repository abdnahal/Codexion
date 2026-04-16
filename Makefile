NAME = Codexion
CC = cc -Wall -Wextra -Werror
SRC = *.c

all:
	$(CC) $(SRC) $(NAME)

clean:
	$(Rm)