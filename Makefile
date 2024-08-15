NAME        :=  webserv
CC          :=  c++
CFLAGS       :=  -Wall -Wextra -Werror -std=c++17 -g -fsanitize=address

INCLUDE_DIRS := -I./include -I./tester
TESTER_MAIN := tester/main.cpp tester/TestCase.cpp tester/TestRequest.cpp
SRC_MAIN := main.cpp

SRC_DIR := .
OBJ_DIR := obj

SRC         :=  $(wildcard src/*.cpp) $(wildcard src/networking/*.cpp) \
	$(wildcard src/request/*.cpp) $(wildcard src/http/*.cpp) $(wildcard src/cgi/*.cpp) $(wildcard src/log/*.cpp)

OBJ         :=  $(SRC:.cpp=.o)

TEST_SRC    :=  $(wildcard tests/*.cpp)
TEST_OBJ    :=  $(TEST_SRC:.cpp=.o)
TEST_NAME   :=  run_tests

# Colors
YELLOW      :=  \033[38;2;204;204;0m
ORANGE      :=  \033[38;2;255;87;34m
RESET       :=  \033[0m

# Cursor manipulation
UP          :=  \033[A
CUT         :=  \033[K
BEGIN       :=  \033[0G

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

$(TEST_NAME): $(OBJ) $(TEST_OBJ)
	@$(CC) $(CFLAGS) $(OBJ) $(TEST_OBJ) -lgtest -lgtest_main -pthread -o $(TEST_NAME)

%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@printf "$(UP)$(BEGIN)$(CUT)$(YELLOW)🧹Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)

fclean:
	@echo "$(UP)$(BEGIN)$(CUT)$(ORANGE)🔥Full clean, removing executable...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@rm -f $(NAME) $(TEST_NAME)

re: fclean all

docker-build:
	@docker build -t server_test .

docker-run:
	@docker run -p 8080:8080 --rm server_test

docker-test: docker-build
	@docker run -p 8080:8080 --rm server_test ./run_tests

.PHONY: all clean fclean re
