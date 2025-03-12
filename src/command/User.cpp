#include "Command_monitor.hpp"

void handle_user_command(int client_socket, const std::vector<std::string>& params, ClientState& client_state) {
    if (params.size() >= 4) {
        client_state.username = params[0];
        client_state.realname = params[3];
    }
}
