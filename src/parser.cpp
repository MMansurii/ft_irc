#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"

int main()
{
    std::string input;
    while (true) {
        std::cout << "Enter an IRC message (or type 'exit' to quit): ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        try {
            IrcMessage msg = Lexer::tokenize(input);
            Parser::parse(msg);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Lexer Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
