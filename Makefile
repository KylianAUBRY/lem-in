NAME	=	lem-in

SRCS	=	src/main.c	\
			src/error.c \
			src/utils.c \
			src/parcing.c \
			src/map_func.c \
			
		
OBJ =	${SRCS:.c=.o}

CC =	cc

CFLAGS	=	-Wall -Wextra -Werror -g #-fsanitize=address

.c.o:
			$(CC) $(CFLAGS) -c $< -o $(<:.c=.o)

$(NAME): ${OBJ}
		$(CC) $(CFLAGS) -o $(NAME) ${OBJ}

all:	${NAME}

clean:
		rm -f ${OBJ}

fclean:	clean
		rm -f ${NAME}

re:	fclean all

.PHONY: all clean fclean re