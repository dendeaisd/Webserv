NAME			:=  webserv
CC          	:=  c++
CFLAGS      	:=  -std=c++17 -g #-Wall -Wextra -Werror
INCLUDE_DIRS	:=	-I./include -I./tester                                  		\
					-I./include/parseConfigFile/semanticAnalysis               		\
					-I./include/parseConfigFile/tokenizing							\
					-I./include/parseConfigFile										
TESTER_MAIN 	:= tester/main.cpp tester/TestCase.cpp tester/TestRequest.cpp
SRC_MAIN 		:= main.cpp

SRC_DIR			:= .
OBJ_DIR			:= obj

SRC         	:=  $(wildcard src/*.cpp) $(wildcard src/networking/*.cpp) 			\
					$(wildcard src/request/*.cpp) $(wildcard src/http/*.cpp)		\
					$(wildcard src/cgi/*.cpp) $(wildcard src/log/*.cpp)             \
					$(wildcard src/response/*.cpp)                                  \
					$(wildcard src/parseConfigFile/*.cpp)                			\
					$(wildcard src/parseConfigFile/semanticAnalysis/*.cpp)          \
					$(wildcard src/parseConfigFile/tokenizing/*.cpp)

OBJ         	:=  $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))
OBJ_MAIN 		:=  $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC_MAIN))
OBJ_TESTER 		:=  $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TESTER_MAIN))
DEP				:=  $(OBJ:.o=.d)

# Suggested usage: make -j <executable>
all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)  # Create necessary directories for object files
	@$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

$(NAME): $(OBJ) $(OBJ_MAIN)
	@$(CC) $(OBJ) $(OBJ_MAIN) -o $(NAME) $(CFLAGS) $(INCLUDE_DIRS) && echo "Compiled $(NAME) successfully..!"
	@touch $(NAME)

test: fclean $(OBJ) $(OBJ_TESTER)
	@$(CC) $(OBJ) $(OBJ_TESTER) -o $(NAME) $(CFLAGS) -D HALT_ON_FAILURE=1 $(INCLUDE_DIRS) && echo "Testing Mode Compiled $(NAME) successfully..!"

.SECONDEXPANSION:
$(OBJ_DIR)/%.d: %.cpp
	@mkdir -p $(dir $@)  # Ensure directory for dependency files exists
	@$(CC) $(CFLAGS) $(INCLUDE_DIRS) -M $< > $@

-include $(DEP)

CONTAINER_NAME := webserv
IMAGE_NAME := webserv
HOST_DIR = $(shell pwd)
CONTAINER_DIR = /usr/src/app

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

docker-build:
	@docker build -t $(IMAGE_NAME) .

docker-run: docker-build
	@docker run --cap-add=NET_ADMIN -it -p 8080:8080 -p 8081:8081 -p 8082:8082 --name $(CONTAINER_NAME) -v $(HOST_DIR):$(CONTAINER_DIR) $(IMAGE_NAME)

docker-ssh:
	@docker exec -it $(CONTAINER_NAME) /bin/bash

docker-purge:
	@docker rm -f $(CONTAINER_NAME) || true

clean:
	@printf "$(UP)$(BEGIN)$(CUT)$(YELLOW)🧹Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d

fclean:
	@echo "$(UP)$(BEGIN)$(CUT)$(ORANGE)🔥Full clean, removing executable...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@rm -f $(NAME) $(TEST_NAME)
	@rm -rf logs
	@rm -rf uploads
	@if [ ! -d "logs" ]; then mkdir logs; fi
	@if [ ! -d "default" ]; then mkdir default; fi
	@if [ ! -d "uploads" ]; then mkdir uploads; fi
	@touch logs/server.log
	@touch logs/error.log

re: fclean all

