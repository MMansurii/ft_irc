#include "Command_monitor.hpp"
#include "ClientState.hpp"

extern ClientState client_state;


using CommandHandler = std::function<void(int, const std::vector<std::string>&, ClientState&)>;
std::map<std::string, CommandHandler> command_handlers;

void initialize_command_handlers() {
    command_handlers["CAP"] = handle_cap_command;
    command_handlers["NICK"] = handle_nick_command;
    command_handlers["USER"] = handle_user_command;
    command_handlers["JOIN"] = handle_join_command;
    command_handlers["PING"] = handle_ping_command;
    command_handlers["QUIT"] = handle_quit_command;
}

void execute_command(int client_socket, IrcMessage& msg, ClientState& client_state) {
    if (command_handlers.empty()) {
        initialize_command_handlers();
    }

    std::string upperCmd = Parser::toUpper(msg.command);
    
    auto it = command_handlers.find(upperCmd);
    if (it != command_handlers.end()) {
        it->second(client_socket, msg.params, client_state);
    } else {
        std::string response = ":irc.example.com 421 " + 
                              (client_state.nickname.empty() ? "*" : client_state.nickname) + 
                              " " + upperCmd + " :Unknown command\r\n";
        send(client_socket, response.c_str(), response.length(), 0);
    }
}
