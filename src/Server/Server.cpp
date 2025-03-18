#include "Server.hpp"
#include "../Lexer/Lexer.hpp"
#include "../Parser/Parser.hpp"
#include "../command/Command_monitor.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>


std::string Server::instance_password = "";


Server::Server(int port, const std::string& password)
    : server_fd(-1), port(port), password(password), running(false) {
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    instance_password = password;
}

Server::~Server() {
    stop();
}

bool Server::initialize() 
{

    int opt = 1;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) 
	{
        std::cerr << "Failed to create socket.\n";
        return false;
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
	{
        std::cerr << "Failed to set socket options.\n";
        close(server_fd);
        server_fd = -1;
        return false;
    }
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
	{
        std::cerr << "Failed to bind socket.\n";
        close(server_fd);
        server_fd = -1;
        return false;
    }
    if (listen(server_fd, 5) < 0)
	{
        std::cerr << "Failed to listen on socket.\n";
        close(server_fd);
        server_fd = -1;
        return false;
    }
    std::cout << "Server is initialized and listening on port " << port << "...\n";
    return true;
}

void Server::send_welcome_messages(int client_socket, const std::string& nickname) 
{
    std::string server_name = "irc.example.com";
    ClientState& client_state = ClientState::getInstance();

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

void Server::handle_client(int client_socket) 
{
    ClientState& client_state = ClientState::getInstance();
    std::string input;
    char buffer[1024];

    while (running) 
	{
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_socket, buffer, sizeof(buffer));
        if (bytes_read <= 0)
		{
            std::cerr << "Client disconnected.\n";
            break;
        }
        input = std::string(buffer, bytes_read);
        std::cout << "Received: " << input << std::endl;
        std::istringstream iss(input);
        std::string line;

        while (std::getline(iss, line, '\n')) 
		{
            if (!line.empty() && line.back() == '\r') 
			{
                line.pop_back();
            }

            if (line.empty()) continue;

            try {
                IrcMessage msg = Lexer::tokenize(line);
                ParseResult result = Parser::parse(msg, client_state);

                if (!result.success) {
                    std::string recipient = client_state.isRegistered() ? client_state.nickname : "*";
                    size_t pos = result.errorMessage.find("* ");
                    if (pos != std::string::npos) {
                        result.errorMessage.replace(pos, 2, recipient + " ");
                    }
                    std::string err = ":irc.example.com " + result.errorMessage + "\r\n";
                    send(client_socket, err.c_str(), err.length(), 0);
                    continue;
                }
                execute_command(client_socket, msg, client_state);
                if (!client_state.isRegistered() &&
                    !client_state.nickname.empty() &&
                    !client_state.username.empty() &&
                    client_state.passwordProvided) 
				{
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

void Server::start() 
{
    if (server_fd == -1) 
	{
        if (!initialize()) 
		{
            std::cerr << "Failed to initialize server.\n";
            return;
        }
    }
    running = true;
    std::cout << "Server started on port " << port << "...\n";

    while (running) 
	{
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            std::cerr << "Failed to accept client connection.\n";
            continue;
        }
        
        std::cout << "Client connected.\n";
        ClientState::getInstance() = ClientState();
        handle_client(client_socket);
    }
}

void Server::stop() 
{
    running = false;
    if (server_fd != -1) {
        close(server_fd);
        server_fd = -1;
        std::cout << "Server stopped.\n";
    }
}
