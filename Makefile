ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME=libft_malloc_$(HOSTTYPE).so
LINK=libft_malloc.so

CC=gcc
CFLAGS=-Wall -Wextra -Werror -fPIC -O2 -g
LDFLAGS=-shared

HEADER=include/header.h
SRC_EXT=c
SRC_DIR=src
SRC=$(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT))

OBJ_DIR=obj
OBJ=$(patsubst $(SRC_DIR)/%.$(SRC_EXT),$(OBJ_DIR)/%.o,$(SRC))

TESTER=tester

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@
	ln -s $(NAME) $(LINK)
	@echo $(LINK) created

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT) $(HEADER) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -r $(OBJ_DIR)
	@echo $(OBJ_DIR) removed

fclean: clean
	rm $(NAME)
	rm $(LINK)
	@echo $(LINK) removed

re: fclean all

tester:
	$(CC) $(CFLAGS) $(TESTER).c -o $(TESTER) -L. -lft_malloc
	@echo $(TESTER) created

.PHONY: all clean fclean re tester
