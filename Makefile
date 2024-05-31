ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME=libft_malloc_$(HOSTTYPE).so
LINK=libft_malloc.so

CC=gcc
CFLAGS=-Wall -Wextra -Werror -fPIC -O2 -g
LDFLAGS=-shared

HEADER_EXT=h
HEADER_DIR=include
HEADER=$(shell find $(HEADER_DIR) -type f -name "*.$(HEADER_EXT)")

SRC_EXT=c
SRC_DIR=src
SRC=$(shell find $(SRC_DIR) -type f -name "*.$(SRC_EXT)")

OBJ_DIR=obj
OBJ=$(patsubst $(SRC_DIR)/%.$(SRC_EXT),$(OBJ_DIR)/%.o,$(SRC))

TESTER=tester
TESTER_FLAGS=-L. -lft_malloc -Wl,-rpath=.

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@
	@ln -sf $(NAME) $(LINK)
	@echo $(NAME) created
	@echo $(LINK) created

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT) $(HEADER) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo $(OBJ_DIR) removed

fclean: clean
	@rm -f $(NAME)
	@rm -f $(LINK)
	@rm -f $(TESTER)
	@echo $(NAME) removed
	@echo $(LINK) removed
	@echo $(TESTER) removed

re: fclean all

tester:
	$(CC) $(CFLAGS) $(TESTER).c -o $(TESTER) $(TESTER_FLAGS)
	@echo $(TESTER) created

.PHONY: all clean fclean re tester
