# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mmansuri <mmansuri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/27 18:45:45 by mmansuri          #+#    #+#              #
#    Updated: 2025/03/19 16:26:27 by ftapponn         ###   ########.fr        #
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
SRCS                = main.cpp \
                      src/utils4main.cpp \
                      src/Server/Server.cpp \
                      src/Server/commands/CAP.cpp \
                      src/Server/commands/JOIN.cpp \
                      src/Server/commands/NICK.cpp \
                      src/Server/commands/PART.cpp \
                      src/Server/commands/PASS.cpp \
                      src/Server/commands/PING.cpp \
                      src/Server/commands/PRIVMSG.cpp \
                      src/Server/commands/QUIT.cpp \
                      src/Server/commands/USER.cpp \
                      src/Server/commands/KICK.cpp \
                      src/Server/commands/INVITE.cpp \
                      src/Server/commands/MODE.cpp \
                      src/Server/commands/TOPIC.cpp \
                      src/Client/Client.cpp \
                      src/Channel/Channel.cpp
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

%.o: %.cpp
	@echo -ne "$(CYAN)Compiling $<... $(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	@echo "$(GREEN)✓$(RESET)"

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
