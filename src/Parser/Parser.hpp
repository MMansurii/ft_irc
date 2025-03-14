#ifndef PARSER_HPP
#define PARSER_HPP

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include "../Lexer/Lexer.hpp"
#include <set>
#include <regex>

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

    static bool validateCommand(const std::string &cmd);
	static bool validateParameters(const std::string &cmd, const std::vector<std::string> &params, bool isOperator);
    static ParseResult parse(const IrcMessage &msg, bool isOperator);
	static std::string toUpper(const std::string &str);

private:
	static bool validateUserAuthentificationParameters(const std::string &cmd, const std::vector<std::string> &params);
	static bool validateUserParameters(const std::string &cmd, const std::vector<std::string> &params);
	static bool validateOperatorParameters(const std::string &cmd, const std::vector<std::string> &params);
	static bool isValidChannel(const std::string &target);
	static bool isValidNickname(const std::string &target);
	static bool validateNicknameLength(const std::string &nickname);
	static bool isValidChannelFormat(const std::string &channel);
	static bool isValidNickFormat(const std::string &nickname);
	static void printError(const std::string &msg);
	static void printParameterError(const std::string &cmd, const std::vector<std::string> &params);
};

#endif
