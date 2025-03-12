#ifndef LEXER_HPP
#define LEXER_HPP

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <string>
#include <vector>
#define MAX_MESSAGE_LENGTH 512

struct IrcMessage 
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
};

class Lexer 
{
public:
    static IrcMessage tokenize(const std::string &input);

private:
    static void handleError(const std::string &message);
};

#endif
