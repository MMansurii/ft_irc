# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mmansuri <mmansuri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/27 18:45:45 by mmansuri          #+#    #+#              #
#    Updated: 2025/03/14 16:35:31 by ftapponn         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME                = ircserv
SRCS                = main.cpp src/utils4main.cpp
CFLAGS              = -Wall -Werror -Wextra -std=c++11
INCLUDE             = -I src/
OBJS                = $(SRCS:.cpp=.o)
CC                  = c++

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(INCLUDE) -o $(NAME)

SRC_lexer            = src/parser.cpp src/Lexer/Lexer.cpp src/Parser/Parser.cpp src/command/Command_monitor.cpp src/command/Join.cpp src/command/Nick.cpp src/command/Ping.cpp src/command/Quit.cpp src/command/User.cpp src/command/Cap.cpp
OBJS_lexer           = $(SRC_lexer:.cpp=.o)
INCLUDE_lexer        = -I src/

lexer: $(OBJS_lexer)
	$(CC) $(CFLAGS) $(OBJS_lexer) $(INCLUDE_lexer) -o parser

clean:
	rm -rf $(OBJS) $(OBJS_lexer)

fclean: clean
	rm -rf $(NAME) parser

re: fclean all

.PHONY: all clean fclean re

