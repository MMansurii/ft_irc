#include "Command_monitor.hpp"
#include "ClientState.hpp"

extern ClientState client_state;

void execute_command(int client_socket, IrcMessage& msg, ClientState& client_state) {
    std::string upperCmd = Parser::toUpper(msg.command);

    if (upperCmd == "CAP") {
        handle_cap_command(client_socket, msg.params, client_state);
    } else if (upperCmd == "NICK") {
        if (!msg.params.empty()) {
            handle_nick_command(client_socket, msg.params[0], client_state);
        }
    } else if (upperCmd == "USER") {
        handle_user_command(client_socket, msg.params, client_state);
    } else if (upperCmd == "JOIN") {
        handle_join_command(client_socket, msg.params, client_state);
    } else if (upperCmd == "PING") {
        handle_ping_command(client_socket, msg.params);
    } else if (upperCmd == "QUIT") {
        handle_quit_command(client_socket);
    } else {
        std::string response = ":irc.example.com 421 " +
                               (client_state.nickname.empty() ? "*" : client_state.nickname) +
                               " " + upperCmd + " :Unknown command\r\n";
        send(client_socket, response.c_str(), response.length(), 0);
    }
}

