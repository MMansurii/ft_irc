#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"

#define PORT 6667
#define PASSWORD "password"

void handle_client(int client_socket) {
    std::string input;
    char buffer[1024];

    send(client_socket, "Welcome to the IRC server!\n", 26, 0);

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_socket, buffer, sizeof(buffer));

        if (bytes_read <= 0) {
            std::cerr << "Client disconnected.\n";
            break;
        }

        input = std::string(buffer, bytes_read);

        if (input == "exit\n") {
            send(client_socket, "Goodbye!\n", 9, 0);
            break;
        }

        try {
            IrcMessage msg = Lexer::tokenize(input);
            
            if (!Parser::validateCommand(msg.command)) {
                throw std::invalid_argument("Invalid IRC command: " + msg.command);
            }
            if (!Parser::validateParameters(msg.command, msg.params)) {
                throw std::invalid_argument("Invalid parameters for command: " + msg.command);
            }

            // If validation passes, proceed to display
            std::string response = "Prefix: " + msg.prefix + "\n";
            response += "Command: " + msg.command + "\n";
            response += "Parameters: ";
            for (const auto& param : msg.params) {
                response += param + " ";
            }
            response += "\n";
            send(client_socket, response.c_str(), response.size(), 0);

        } catch (const std::invalid_argument& e) {
            std::string error_msg = "Error: " + std::string(e.what()) + "\n";
            send(client_socket, error_msg.c_str(), error_msg.size(), 0);
        }
    }

    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket.\n";
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket.\n";
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Failed to listen on socket.\n";
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << "...\n";

    while (true) {
        client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            std::cerr << "Failed to accept client connection.\n";
            continue;
        }

        std::cout << "Client connected.\n";

        handle_client(client_socket);
    }

    close(server_fd);
    return 0;
}

