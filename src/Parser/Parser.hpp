#ifndef PARSER_HPP
#define PARSER_HPP
#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include "../Lexer/Lexer.hpp"
#include <set>
#include <regex>

class Parser 
{
public:
    static bool validateCommand(const std::string &cmd);
    static void parse(const IrcMessage &msg);
    static bool validateParameters(const std::string &cmd, const std::vector<std::string> &params);
    static std::string toUpper(const std::string &str);
private:
    static bool isValidChannel(const std::string &target);
    static bool isValidNickname(const std::string &target);
    static bool validateNicknameLength(const std::string &nickname);
    static bool isValidChannelFormat(const std::string &channel);
    static bool isValidNickFormat(const std::string &nickname);
    static void printError(const std::string &msg);
    static void printParameterError(const std::string &cmd, const std::vector<std::string> &params);
};
#endif
