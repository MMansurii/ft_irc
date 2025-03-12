#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "command/Command_monitor.hpp"
#define PORT 6667
#define PASSWORD "password"
#include "command/ClientState.hpp"

void handle_cap_command(int client_socket, const std::vector<std::string>& params);
void handle_nick_command(int client_socket, const std::string& nick);
void handle_user_command(int client_socket, const std::vector<std::string>& params);
void handle_join_command(int client_socket, const std::vector<std::string>& params);

;
extern ClientState client_state;

ClientState client_state;

void send_welcome_messages(int client_socket, const std::string& nickname) {
    std::string server_name = "irc.example.com";
    
    std::string welcome = ":" + server_name + " 001 " + nickname + " :Welcome to the IRC Network " + 
                          nickname + "!" + client_state.username + "@localhost\r\n";
    send(client_socket, welcome.c_str(), welcome.length(), 0);
    
    std::string yourhost = ":" + server_name + " 002 " + nickname + 
                           " :Your host is " + server_name + ", running version 1.0\r\n";
    send(client_socket, yourhost.c_str(), yourhost.length(), 0);
    
    std::string created = ":" + server_name + " 003 " + nickname + 
                           " :This server was created March 2025\r\n";
    send(client_socket, created.c_str(), created.length(), 0);
    
    std::string myinfo = ":" + server_name + " 004 " + nickname + 
                         " " + server_name + " 1.0 o o\r\n";
    send(client_socket, myinfo.c_str(), myinfo.length(), 0);
}

void handle_client(int client_socket) {
    std::string input;
    char buffer[1024];
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_socket, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
            std::cerr << "Client disconnected.\n";
            break;
        }
        
        input = std::string(buffer, bytes_read);
        std::cout << "Received: " << input << std::endl;
        
        std::istringstream iss(input);
        std::string line;
        
        while (std::getline(iss, line, '\n')) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            
            if (line.empty()) continue;
            
            try {
                IrcMessage msg = Lexer::tokenize(line);
                
                if (!Parser::validateCommand(msg.command)) {
                    std::string err = ":irc.example.com 421 " + 
                                     (client_state.nickname.empty() ? "*" : client_state.nickname) + 
                                     " " + msg.command + " :Unknown command\r\n";
                    send(client_socket, err.c_str(), err.length(), 0);
                    continue;
                }

                if (!Parser::validateParameters(msg.command, msg.params)) {
                    std::string err = ":irc.example.com 461 " + 
                                     (client_state.nickname.empty() ? "*" : client_state.nickname) + 
                                     " " + msg.command + " :Invalid parameters\r\n";
                    send(client_socket, err.c_str(), err.length(), 0);
                    continue;
                }
                
                Parser::parse(msg);
                std::string upperCmd = Parser::toUpper(msg.command);
				execute_command(client_socket, msg, client_state);
                
				if (!client_state.registered && 
                    !client_state.nickname.empty() && 
                    !client_state.username.empty()) {
                    client_state.registered = true;
                    send_welcome_messages(client_socket, client_state.nickname);
                }
                
            } catch (const std::exception& e) {
                std::cerr << "Error processing command: " << e.what() << std::endl;
                std::string err = ":irc.example.com 451 * :Error processing command\r\n";
                send(client_socket, err.c_str(), err.length(), 0);
            }
        }
    }
    close(client_socket);
}

int main() 
{
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket.\n";
        return -1;
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "Failed to set socket options.\n";
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
        client_state = ClientState();
        handle_client(client_socket);
    }
    
    close(server_fd);
    return 0;
}
