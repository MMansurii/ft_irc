# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mmansuri <mmansuri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/27 18:45:45 by mmansuri          #+#    #+#              #
#    Updated: 2025/02/28 16:26:29 by mmansuri         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME                = ircserv
SRCS                = main.cpp
CFLAGS              = -Wall -Werror -Wextra -std=c++11
OBJS                = $(SRCS:%.cpp=%.o)
CC                  = c++

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(SRCS) -o $(NAME)

# Lexer and parser rules
SRC_lexer            = src/parser.cpp src/Lexer/Lexer.cpp src/Parser/Parser.cpp
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

