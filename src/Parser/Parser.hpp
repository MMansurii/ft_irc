#ifndef PARSER_HPP
#define PARSER_HPP

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include "../Lexer/Lexer.hpp"
#include <set>
#include <regex>
#include "../command/ClientState.hpp"

enum class ParseErrorType {
    NONE,
    INVALID_COMMAND,
    INVALID_PARAMETERS,
    PARSING_ERROR
};

struct ParseResult {
    bool success;
    ParseErrorType errorType;
    std::string errorMessage;
    
    ParseResult() : success(true), errorType(ParseErrorType::NONE) {}
    
    static ParseResult Success() {
        return ParseResult();
    }
    
    static ParseResult Error(ParseErrorType type, const std::string& message) {
        ParseResult result;
        result.success = false;
        result.errorType = type;
        result.errorMessage = message;
        return result;
    }
};

class Parser
{
public:
    static ParseResult parse(const IrcMessage &msg, const ClientState &clientState);
    static std::string toUpper(const std::string &str);
private:
    static bool validateCommand(const std::string &cmd);
	static bool validateParameters(const std::string &cmd, const std::vector<std::string> &params, const ClientState &clientState);
    static bool validateUserAuthentificationParameters(const std::string &cmd, const std::vector<std::string> &params);
    static bool validateUserParameters(const std::string &cmd, const std::vector<std::string> &params);
    static bool validateOperatorParameters(const std::string &cmd, const std::vector<std::string> &params);
    static bool isValidChannel(const std::string &target);
    static bool isValidNickname(const std::string &target);
};
#endif
