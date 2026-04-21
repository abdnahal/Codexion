NAME = Codexion
CC = cc -Wall -Wextra -Werror -pthread
SRC = *.c
RM = rm -f


all: $(NAME)


$(NAME):
	$(CC) $(SRC) -o $(NAME)

clean:
	$(RM) *.o

fclean: clean
	$(RM) $(NAME)

re: fclean all