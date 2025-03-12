#include "Lexer.hpp"
#include <sstream>


void Lexer::handleError(const std::string &message) {
    std::cerr << "Lexer Error: " << message << std::endl;
}


IrcMessage Lexer::tokenize(const std::string &input)
{
    IrcMessage msg;
    std::string processedInput = input;

    processedInput.erase(0, processedInput.find_first_not_of(" \t\r\n"));
    processedInput.erase(processedInput.find_last_not_of(" \t\r\n") + 1);
    std::istringstream iss(processedInput);
    std::string word;

    if (processedInput.empty()) {
        throw std::invalid_argument("Empty message received");
    }

    if (processedInput[0] == ':') {
        iss >> word;
        msg.prefix = word.substr(1);
    }

    if (!(iss >> msg.command)) {
        throw std::invalid_argument("Invalid or missing command");
    }

    while (iss >> word) {
        if (word[0] == ':') {
            std::string trailing = word.substr(1);
            std::string temp;
            if (getline(iss, temp)) {
                trailing += temp;
            }
            msg.params.push_back(trailing);
            break;
        }
        msg.params.push_back(word);
    }

    return msg;
}
