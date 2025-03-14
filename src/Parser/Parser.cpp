#include "Parser.hpp"
#include <regex>

std::string Parser::toUpper(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Parser::validateCommand(const std::string &cmd)
{
    static std::set<std::string> validCommands;
    if (validCommands.empty()) {
        validCommands.insert("PASS");
        validCommands.insert("NICK");
        validCommands.insert("USER");
        validCommands.insert("JOIN");
        validCommands.insert("PART");
        validCommands.insert("PRIVMSG");
        validCommands.insert("KICK");
        validCommands.insert("INVITE");
        validCommands.insert("TOPIC");
        validCommands.insert("MODE");
        validCommands.insert("QUIT");
        validCommands.insert("CAP");
    }
    if (cmd.length() == 3 && std::isdigit(cmd[0]) && std::isdigit(cmd[1]) && std::isdigit(cmd[2])) {
        return true;
    }
    std::string upperCmd = toUpper(cmd);
    return validCommands.count(upperCmd) > 0;
}

bool Parser::validateParameters(const std::string &cmd, const std::vector<std::string> &params, bool isOperator)
{
	std::string upperCmd = toUpper(cmd);

	bool isAuthentified = true;
	if (validateUserAuthentificationParameters(upperCmd, params))
		return true;
	if (validateUserParameters(upperCmd, params)) 
	{
		if(!isAuthentified)
		{
			std::cerr << "You have to register" << std::endl; 
			return false;
		}
		return true;
	}
	if (validateOperatorParameters(upperCmd, params)) 
	{
		if(!isOperator)
		{
			std::cerr << "You are not have operator privilege" << std::endl; 
			return false;
		}
		if(!isAuthentified)
		{
			std::cerr << "You have to register" << std::endl; 
			return false;
		}
		return true;
	}

	return false;
}


bool Parser::validateUserAuthentificationParameters(const std::string &cmd, const std::vector<std::string> &params)
{
	if (cmd == "PASS") 
        return params.size() == 1;
    else if (cmd == "NICK") 
        return params.size() == 1 && isValidNickname(params[0]);
    else if (cmd == "USER") 
        return params.size() >= 4 && isValidNickname(params[0]);
	return false;
}
bool Parser::validateUserParameters(const std::string &cmd, const std::vector<std::string> &params)
{
    
	if (cmd == "JOIN")
        return params.size() == 1 && isValidChannel(params[0]);
    else if (cmd == "PART")
        return params.size() >= 1 && isValidChannel(params[0]);
    else if (cmd == "PRIVMSG" || cmd == "NOTICE")
	{
        if (params.size() < 2 || params[1].empty())
            return false;
        return isValidChannel(params[0]) || isValidNickname(params[0]);
    }
    else if (cmd == "QUIT") 
        return params.size() <= 1;
    else if (cmd == "PING" || cmd == "PONG") 
        return params.size() >= 1 && !params[0].empty();
    else if (cmd == "CAP")
        return params.size() >= 1;

    return false;
}

bool Parser::validateOperatorParameters(const std::string &cmd, const std::vector<std::string> &params)
{
    if (cmd == "KICK") 
        return params.size() >= 2 && isValidChannel(params[0]) && isValidNickname(params[1]);
    else if (cmd == "INVITE")
        return params.size() == 2 && isValidNickname(params[0]) && isValidChannel(params[1]);
    else if (cmd == "TOPIC")
        return params.size() >= 1 && isValidChannel(params[0]);
    else if (cmd == "MODE")
        return params.size() >= 1;
    return false;
}

bool Parser::isValidChannel(const std::string &target)
{
    std::regex channelPattern("^#[^ \x07\r\n\\0]{1,49}$");
    return std::regex_match(target, channelPattern);
}


bool Parser::isValidNickname(const std::string &target)
{
    std::regex nicknamePattern("^[A-Za-z_][A-Za-z0-9_]{0,8}$");
    return std::regex_match(target, nicknamePattern);
}

ParseResult Parser::parse(const IrcMessage &msg, bool isOperator)
{
    try 
	{
        if (!validateCommand(msg.command)) 
		{
            return ParseResult::Error(
                ParseErrorType::INVALID_COMMAND,
                "421 * " + msg.command + " :Unknown command"
            );
        }
        if (!validateParameters(msg.command, msg.params, isOperator)) 
		{
            return ParseResult::Error(
                ParseErrorType::INVALID_PARAMETERS,
                "461 * " + msg.command + " :Invalid parameters"
            );
        }
        std::cout << "Command: " << msg.command << std::endl;
        if (!msg.prefix.empty())
            std::cout << "Prefix: " << msg.prefix << std::endl;
        for (size_t i = 0; i < msg.params.size(); ++i)
            std::cout << "Param " << i + 1 << ": " << msg.params[i] << std::endl;
        return ParseResult::Success();
    } 
	catch (const std::exception &e) 
	{
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return ParseResult::Error(
            ParseErrorType::PARSING_ERROR,
            "451 * :Error processing command: " + std::string(e.what())
        );
    }
}
