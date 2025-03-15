# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mmansuri <mmansuri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/27 18:45:45 by mmansuri          #+#    #+#              #
#    Updated: 2025/03/15 19:51:58 by ftapponn         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #
GREEN = \033[0;32m
BLUE = \033[0;34m
RED = \033[0;31m
YELLOW = \033[0;33m
PURPLE = \033[0;35m
CYAN = \033[0;36m
WHITE = \033[1;37m
RESET = \033[0m

                                      
define FT_IRC_BANNER

$(CYAN)  ______   _______     _____  _____   _____ 
$(CYAN) |  ____| |__   __|   |_   _||  __ \ / ____|
$(PURPLE)| |__       | |        | |  | |__) | |     
$(PURPLE)|  __|      | |        | |  |  _  /| |     
$(BLUE)| |         | |       _| |_ | | \ \| |____ 
$(BLUE)|_|         |_|      |_____||_|  \_\\_____|
$(YELLOW)=============================================$(RESET)
                                       
endef
export FT_IRC_BANNER

NAME                = ircserv
SRCS                = main.cpp src/utils4main.cpp
CFLAGS              = -Wall -Werror -Wextra -std=c++17
INCLUDE             = -I src/
OBJS                = $(SRCS:.cpp=.o)
CC                  = c++

LOADING_CHARS       = ⠋ ⠙ ⠹ ⠸ ⠼ ⠴ ⠦ ⠧ ⠇ ⠏

all: pre_build $(NAME)
	@echo "$(GREEN)✓ Build complete! $(RESET)"
	@echo "$(YELLOW)Run with: $(WHITE)./$(NAME)$(RESET)"

pre_build:
	@clear
	@echo "$$FT_IRC_BANNER"
	@echo "$(BLUE)Starting build process...$(RESET)"
	@sleep 0.5

$(NAME): $(OBJS)
	@echo "$(YELLOW)Linking objects...$(RESET)"
	@for i in $(LOADING_CHARS); do \
		echo -ne "\r$(YELLOW)Linking... $$i $(RESET)"; \
		sleep 0.1; \
	done
	@echo -ne "\r$(GREEN)Linking... ✓ $(RESET)\n"
	@$(CC) $(CFLAGS) $(OBJS) $(INCLUDE) -o $(NAME)
	@echo "$(GREEN)✓ Created executable: $(WHITE)$(NAME)$(RESET)"

SRC_lexer           = src/parser.cpp src/Lexer/Lexer.cpp src/Parser/Parser.cpp src/command/Command_monitor.cpp src/command/Join.cpp src/command/Nick.cpp src/command/Ping.cpp src/command/Quit.cpp src/command/User.cpp src/command/Cap.cpp
OBJS_lexer          = $(SRC_lexer:.cpp=.o)
INCLUDE_lexer       = -I src/

%.o: %.cpp
	@echo -ne "$(CYAN)Compiling $<... $(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(GREEN)✓$(RESET)"

lexer: pre_lexer $(OBJS_lexer)
	@echo "$(YELLOW)Linking lexer objects...$(RESET)"
	@for i in $(LOADING_CHARS); do \
		echo -ne "\r$(YELLOW)Linking... $$i $(RESET)"; \
		sleep 0.1; \
	done
	@echo -ne "\r$(GREEN)Linking... ✓ $(RESET)\n"
	@$(CC) $(CFLAGS) $(OBJS_lexer) $(INCLUDE_lexer) -o parser
	@echo "$(GREEN)✓ Created parser utility: $(WHITE)parser$(RESET)"

pre_lexer:
	@echo "$(BLUE)Building lexer components...$(RESET)"
	@sleep 0.5

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@for i in $(LOADING_CHARS); do \
		echo -ne "\r$(RED)Cleaning... $$i $(RESET)"; \
		sleep 0.1; \
	done
	@rm -rf $(OBJS) $(OBJS_lexer)
	@echo -ne "\r$(GREEN)Cleaning... ✓ $(RESET)\n"
	@echo "$(GREEN)✓ Removed object files$(RESET)"

fclean: clean
	@echo "$(RED)Removing executables...$(RESET)"
	@for i in $(LOADING_CHARS); do \
		echo -ne "\r$(RED)Removing... $$i $(RESET)"; \
		sleep 0.1; \
	done
	@rm -rf $(NAME) parser
	@echo -ne "\r$(GREEN)Removing... ✓ $(RESET)\n"
	@echo "$(GREEN)✓ Removed executables: $(WHITE)$(NAME) parser$(RESET)"

re: fclean all

help:
	@echo "$(CYAN)Available commands:$(RESET)"
	@echo "  $(WHITE)make$(RESET)        - Build the IRC server"
	@echo "  $(WHITE)make lexer$(RESET)  - Build the parser utility"
	@echo "  $(WHITE)make clean$(RESET)  - Remove object files"
	@echo "  $(WHITE)make fclean$(RESET) - Remove object files and executables"
	@echo "  $(WHITE)make re$(RESET)     - Rebuild everything"
	@echo "  $(WHITE)make help$(RESET)   - Display this help message"

.PHONY: all clean fclean re help pre_build pre_lexer
