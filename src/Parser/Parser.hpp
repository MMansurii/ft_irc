#ifndef PARSER_HPP
#define PARSER_HPP

#include "../Lexer/Lexer.hpp"
#include <iostream>
#include <set>

class Parser {
public:
    static bool validateCommand(const std::string &cmd);
    static void parse(const IrcMessage &msg);
};

#endif

