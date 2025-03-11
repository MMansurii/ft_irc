#include "Parser.hpp"

bool Parser::validateCommand(const std::string &cmd)
{
    static std::set<std::string> validCommands;
    if (validCommands.empty()) {
        validCommands.insert("CAP");
        validCommands.insert("PASS");
        validCommands.insert("NICK");
        validCommands.insert("USER");
        validCommands.insert("PING");
        validCommands.insert("PONG");
        validCommands.insert("JOIN");
        validCommands.insert("PART");
        validCommands.insert("PRIVMSG");
        validCommands.insert("NOTICE");
        validCommands.insert("KICK");
        validCommands.insert("INVITE");
        validCommands.insert("TOPIC");
        validCommands.insert("MODE");
        validCommands.insert("QUIT");
    }
    if (cmd.length() == 3 && std::isdigit(cmd[0]) && std::isdigit(cmd[1]) && std::isdigit(cmd[2]))
    {
        return true;
    }
    std::string upperCmd = toUpper(cmd);
    return validCommands.count(upperCmd) > 0;
}



bool Parser::validateParameters(const std::string &cmd, const std::vector<std::string> &params)
{
    std::string upperCmd = toUpper(cmd);
    
    if (upperCmd == "NICK") {
        return params.size() == 1 && !params[0].empty() && params[0].length() <= 9;
    }
    else if (upperCmd == "USER") {
        return params.size() >= 4;
    }
    else if (upperCmd == "JOIN") {
        return params.size() >= 1 && !params[0].empty();
    }
    else if (upperCmd == "PRIVMSG" || upperCmd == "NOTICE") {
        return params.size() >= 2 && !params[0].empty();
    }
    else if (upperCmd == "PING" || upperCmd == "PONG") {
        return params.size() >= 1;
    }
    
    return true;
}

std::string Parser::toUpper(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

void Parser::parse(const IrcMessage &msg)
{
    if (!validateCommand(msg.command)) {
        std::cerr << "Invalid IRC command: " << msg.command << std::endl;
        return;
    }

    if (!validateParameters(msg.command, msg.params)) {
        std::cerr << "Invalid parameters for command: " << msg.command << std::endl;
        return;
    }

    std::cout << "Command: " << msg.command << std::endl;

    if (!msg.prefix.empty())
        std::cout << "Prefix: " << msg.prefix << std::endl;

    for (size_t i = 0; i < msg.params.size(); ++i)
        std::cout << "Param " << i + 1 << ": " << msg.params[i] << std::endl;
}
