#ifndef COMMAND_MONITOR_HPP
#define COMMAND_MONITOR_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include "../Lexer/Lexer.hpp"
#include "ClientState.hpp"
#include "../Parser/Parser.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <string>    

void execute_command(int client_socket, IrcMessage& msg, ClientState& client_state);

void handle_cap_command(int client_socket, const std::vector<std::string>& params, ClientState& client_state);
void handle_nick_command(int client_socket, const std::string& nick, ClientState& client_state);
void handle_user_command(int client_socket, const std::vector<std::string>& params, ClientState& client_state);
void handle_join_command(int client_socket, const std::vector<std::string>& params, ClientState& client_state);
void handle_ping_command(int client_socket, const std::vector<std::string>& params);
void handle_quit_command(int client_socket);

#endif 
