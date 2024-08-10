NAME        :=  server_test
CC          :=  c++
FLAGS       :=  -Wall -Wextra -Werror -std=c++17 -g

SRC         :=  $(wildcard src/*.cpp) $(wildcard src/networking/*.cpp)
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

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME)

$(TEST_NAME): $(OBJ) $(TEST_OBJ)
	@$(CC) $(FLAGS) $(OBJ) $(TEST_OBJ) -lgtest -lgtest_main -pthread -o $(TEST_NAME)

%.o: %.cpp
	@$(CC) $(FLAGS) -c $< -o $@

clean:
	@printf "$(UP)$(BEGIN)$(CUT)$(YELLOW)🧹Cleaning object files...$(RESET)"
	@rm -rf $(OBJ)

fclean:
	@printf "$(UP)$(BEGIN)$(CUT)$(ORANGE)🔥Full clean, removing executable...$(RESET)"
	@rm -rf $(OBJ)
	@rm -f $(NAME) $(TEST_NAME)

re: fclean all

docker-build:
	@docker build -t server_test .

docker-run:
	@docker run -p 8080:8080 --rm server_test

docker-test: docker-build
	@docker run -p 8080:8080 --rm server_test ./run_tests

.PHONY: all clean fclean re
