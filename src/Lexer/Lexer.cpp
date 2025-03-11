#include "Lexer.hpp"
#include <sstream>

IrcMessage Lexer::tokenize(const std::string &input) {
    IrcMessage msg;
    std::istringstream iss(input);
    std::string word;

    // Extract prefix if present
    if (!input.empty() && input[0] == ':') {
        iss >> word;
        msg.prefix = word.substr(1); // Remove leading ':'
    }

    // Extract command
    if (iss >> msg.command) {
        // Extract parameters
        while (iss >> word) {
            if (word[0] == ':') { // Trailing parameter
                std::string trailing;
                getline(iss, trailing);
                msg.params.push_back(word.substr(1) + trailing);
                break;
            }
            msg.params.push_back(word);
        }
    }

    return msg;
}
