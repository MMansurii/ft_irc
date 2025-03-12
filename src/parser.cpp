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

#define PORT 6667
#define PASSWORD "password"

// Forward declarations
void handle_cap_command(int client_socket, const std::vector<std::string>& params);
void handle_nick_command(int client_socket, const std::string& nick);
void handle_user_command(int client_socket, const std::vector<std::string>& params);
void handle_join_command(int client_socket, const std::vector<std::string>& params);

// Structure to keep track of client state
struct ClientState {
    std::string nickname;
    std::string username;
    std::string realname;
    bool registered = false;
    std::vector<std::string> channels;
};

// Map to store client states (in a real app, you'd use multiple clients)
ClientState client_state;

void send_welcome_messages(int client_socket, const std::string& nickname) {
    // Send numeric replies according to RFC 1459
    std::string server_name = "irc.example.com";
    
    // 001 RPL_WELCOME
    std::string welcome = ":" + server_name + " 001 " + nickname + " :Welcome to the IRC Network " + 
                          nickname + "!" + client_state.username + "@localhost\r\n";
    send(client_socket, welcome.c_str(), welcome.length(), 0);
    
    // 002 RPL_YOURHOST
    std::string yourhost = ":" + server_name + " 002 " + nickname + 
                           " :Your host is " + server_name + ", running version 1.0\r\n";
    send(client_socket, yourhost.c_str(), yourhost.length(), 0);
    
    // 003 RPL_CREATED
    std::string created = ":" + server_name + " 003 " + nickname + 
                           " :This server was created March 2025\r\n";
    send(client_socket, created.c_str(), created.length(), 0);
    
    // 004 RPL_MYINFO
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
        
        // Split the input by \r\n to handle multiple commands in one packet
        std::istringstream iss(input);
        std::string line;
        
        while (std::getline(iss, line, '\n')) {
            // Remove \r if present
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            
            if (line.empty()) continue;
            
            try {
                IrcMessage msg = Lexer::tokenize(line);
                
                if (!Parser::validateCommand(msg.command)) {
                    std::cerr << "Invalid command: " << msg.command << std::endl;
                    continue;
                }
                
                // Handle specific commands
                if (msg.command == "CAP") {
                    handle_cap_command(client_socket, msg.params);
                } 
                else if (msg.command == "NICK") {
                    if (msg.params.size() >= 1) {
                        handle_nick_command(client_socket, msg.params[0]);
                    }
                } 
                else if (msg.command == "USER") {
                    handle_user_command(client_socket, msg.params);
                } 
                else if (msg.command == "JOIN") {
                    handle_join_command(client_socket, msg.params);
                } 
                else if (msg.command == "PING") {
                    // Respond to PING with PONG
                    std::string pong = "PONG :" + (msg.params.size() >= 1 ? msg.params[0] : "irc.example.com") + "\r\n";
                    send(client_socket, pong.c_str(), pong.length(), 0);
                } 
                else if (msg.command == "QUIT") {
                    std::cout << "Client requested to quit." << std::endl;
                    return;
                } 
                else {
                    // Unsupported command
                    std::string response = ":" + std::string("irc.example.com") + " 421 " + 
                                           client_state.nickname + " " + msg.command + " :Unknown command\r\n";
                    send(client_socket, response.c_str(), response.length(), 0);
                }
                
                // Check if we can register the user now
                if (!client_state.registered && 
                    !client_state.nickname.empty() && 
                    !client_state.username.empty()) {
                    client_state.registered = true;
                    send_welcome_messages(client_socket, client_state.nickname);
                }
                
            } catch (const std::exception& e) {
                std::cerr << "Error processing command: " << e.what() << std::endl;
            }
        }
    }
    
    close(client_socket);
}

void handle_cap_command(int client_socket, const std::vector<std::string>& params) {
    if (params.size() >= 1) {
        if (params[0] == "LS") {
            // Send an empty capability list according to IRCv3 spec
            std::string response = ":irc.example.com CAP * LS :\r\n";
            send(client_socket, response.c_str(), response.length(), 0);
        } else if (params[0] == "END") {
            // Client is done with capability negotiation
            return;
        } else if (params[0] == "REQ") {
            // Acknowledge capabilities (simplified - we don't actually support any)
            if (params.size() >= 2) {
                std::string cap_ack = ":irc.example.com CAP * ACK :" + params[1] + "\r\n";
                send(client_socket, cap_ack.c_str(), cap_ack.length(), 0);
            }
        } else if (params[0] == "LIST") {
            // List active capabilities (empty in this case)
            std::string response = ":irc.example.com CAP * LIST :\r\n";
            send(client_socket, response.c_str(), response.length(), 0);
        }
    }
}

void handle_nick_command(int client_socket, const std::string& nick) {
    // Store the nickname
    client_state.nickname = nick;
    
    // Acknowledge the NICK command
    std::string nick_reply = ":" + (client_state.nickname.empty() ? "*" : client_state.nickname) + 
                             "!user@localhost NICK :" + nick + "\r\n";
    send(client_socket, nick_reply.c_str(), nick_reply.length(), 0);
}

void handle_user_command(int client_socket, const std::vector<std::string>& params) {
    if (params.size() >= 4) {
        client_state.username = params[0];
        client_state.realname = params[3];
        // USER command doesn't need an acknowledgment as the welcome messages will follow
    }
}

void handle_join_command(int client_socket, const std::vector<std::string>& params) {
    if (!client_state.registered) {
        // Cannot join channels before registration
        std::string err = ":irc.example.com 451 * :You have not registered\r\n";
        send(client_socket, err.c_str(), err.length(), 0);
        return;
    }
    
    if (params.size() >= 1) {
        std::string channels = params[0];
        std::istringstream channel_stream(channels);
        std::string channel;
        
        // Split by commas for multiple channels
        while (std::getline(channel_stream, channel, ',')) {
            if (channel.empty() || channel[0] != '#') {
                continue;  // Invalid channel name
            }
            
            // Add channel to client's joined channels
            client_state.channels.push_back(channel);
            
            // Send JOIN confirmation
            std::string join_msg = ":" + client_state.nickname + "!user@localhost JOIN " + channel + "\r\n";
            send(client_socket, join_msg.c_str(), join_msg.length(), 0);
            
            // Send topic (no topic in this simple server)
            std::string topic_msg = ":irc.example.com 331 " + client_state.nickname + " " + channel + " :No topic is set\r\n";
            send(client_socket, topic_msg.c_str(), topic_msg.length(), 0);
            
            // Send names list (just the joining user in this simple server)
            std::string names_prefix = ":irc.example.com 353 " + client_state.nickname + " = " + channel + " :@" + client_state.nickname + "\r\n";
            send(client_socket, names_prefix.c_str(), names_prefix.length(), 0);
            
            // End of names list
            std::string names_end = ":irc.example.com 366 " + client_state.nickname + " " + channel + " :End of /NAMES list.\r\n";
            send(client_socket, names_end.c_str(), names_end.length(), 0);
        }
    }
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
    
    // Allow socket reuse to avoid "address already in use" errors
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
        
        // Reset client state for new connection
        client_state = ClientState();
        
        handle_client(client_socket);
    }
    
    close(server_fd);
    return 0;
}
