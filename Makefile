NAME=libft.a

CC=gcc
CFLAGS=-Wall -Wextra -Werror

SRC_EXT=c
SRC_DIR=src
SRC=$(shell find $(SRC_DIR) -type f -name "*.$(SRC_EXT)")

OBJ=$(subst $(SRC_DIR),$(OBJ_DIR),$(SRC:.c=.o))
OBJ_DIR=obj

RM=rm -rf

all: ${NAME}

${NAME}: $(OBJ_DIR) ${OBJ}
	ar rcs ${NAME} ${OBJ}

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT) ${HEADER}
	$(CC) $(CFLAGS) -c $< -o $(<:.$(SRC_EXT)=.o)
	@mv $(SRC_DIR)/*.o $@

clean:
	${RM} ${OBJ_DIR}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re
