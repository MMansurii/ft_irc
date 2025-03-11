#include "Parser.hpp"

bool Parser::validateCommand(const std::string &cmd) {
    static std::set<std::string> validCommands;
    if (validCommands.empty()) {  // Initialize only once
        validCommands.insert("NICK");
        validCommands.insert("USER");
        validCommands.insert("JOIN");
        validCommands.insert("PART");
        validCommands.insert("PRIVMSG");
        validCommands.insert("QUIT");
    }
    return validCommands.count(cmd) > 0;
}

void Parser::parse(const IrcMessage &msg) {
    if (!validateCommand(msg.command)) {
        std::cerr << "Invalid IRC command: " << msg.command << std::endl;
        return;
    }

    std::cout << "Command: " << msg.command << std::endl;
    if (!msg.prefix.empty())
        std::cout << "Prefix: " << msg.prefix << std::endl;

    for (size_t i = 0; i < msg.params.size(); ++i)
        std::cout << "Param " << i + 1 << ": " << msg.params[i] << std::endl;
}
