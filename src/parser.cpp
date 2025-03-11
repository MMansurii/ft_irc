#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"

int main() {
    std::string input;
    
    std::cout << "Valid IRC commands: NICK, USER, JOIN, PART, PRIVMSG, QUIT" << std::endl;
    std::cout << "Example: PRIVMSG #channel :Hello, world!" << std::endl;
    
    while (true) {
        std::cout << "Enter an IRC message (or type 'exit' to quit): ";
        std::getline(std::cin, input);
        
        if (input == "exit") {
            std::cout << "Exiting...\n";
            break;
        }

        IrcMessage msg = Lexer::tokenize(input);
        Parser::parse(msg);
    }

    return 0;
}
