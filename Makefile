NAME := webserv

CC := c++
CFLAGS := -Werror -Wextra -Wall -std=c++98
INCLUDE_DIRS := -I./include -I./tester
TESTER_MAIN := tester/main.cpp tester/TestCase.cpp tester/TestRequest.cpp
SRC_MAIN := main.cpp
SRC := src/http/HttpMaps.cpp src/request/HttpRequest.cpp src/request/HttpRequestParser.cpp

SRC_DIR := .
OBJ_DIR := obj

OBJ := $(addprefix $(OBJ_DIR)/, $(subst /,@,$(SRC:.cpp=.o)))
OBJ_MAIN := $(addprefix $(OBJ_DIR)/, $(subst /,@,$(SRC_MAIN:.cpp=.o)))
OBJ_TESTER := $(addprefix $(OBJ_DIR)/, $(subst /,@,$(TESTER_MAIN:.cpp=.o)))

all:
	@$(MAKE) $(NAME) -j

$(NAME): $(OBJ) $(OBJ_MAIN)
	@$(CC) $(OBJ) $(OBJ_MAIN) -o $(NAME) $(CFLAGS) $(INCLUDE_DIRS) && echo "Compiled $(NAME) successfully..!"

test: fclean $(OBJ) $(OBJ_TESTER)
	@$(CC) $(OBJ) $(OBJ_TESTER) -o $(NAME) $(CFLAGS) -D HALT_ON_FAILURE=1 $(INCLUDE_DIRS) && echo "Testing Mode Compiled $(NAME) successfully..!"

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
