NAME := webserv

CC := c++
CFLAGS := -Werror -Wextra -Wall -std=c++98
INCLUDE_DIRS := -I./include
SRC := main.cpp src/http/HttpMaps.cpp src/request/HttpRequest.cpp src/request/HttpRequestParser.cpp

SRC_DIR := .
OBJ_DIR := obj

OBJ := $(addprefix $(OBJ_DIR)/, $(subst /,@,$(SRC:.cpp=.o)))

all:
	@$(MAKE) $(NAME) -j

$(NAME): $(OBJ)
	@$(CC) $(OBJ) -o $(NAME) $(CFLAGS) $(INCLUDE_DIRS) && echo "Compiled $(NAME) successfully..!"

## OBJ_DIR can be replaced by INCLUDE directory incase you need to include a library
.SECONDEXPANSION:
$(OBJ_DIR)/%.o: $(SRC_DIR)/$$(subst @,/,$$*).cpp
	@$(CC) $(CFLAGS) $(addprefix -iquote ,$(OBJ_DIR)) -c $< -o $@

$(OBJ): $(OBJ_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(OBJ_DIR)
	@rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean re
