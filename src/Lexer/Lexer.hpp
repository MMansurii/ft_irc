#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

struct IrcMessage {
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
};

class Lexer {
public:
    static IrcMessage tokenize(const std::string &input);
};

#endif

