#include "Command_monitor.hpp"

void handle_privmsg_command(int client_socket, const std::vector<std::string>& params, ClientState& client_state) {

    std::string target = params[0];
    std::string message = params[1];
    
    if (target[0] == '#') {
        // Target is a channel
        // Here you would check if the channel exists and if the user is in the channel
        // Then forward the message to all users in the channel
        
        // Placeholder for channel message forwarding
        std::cout << "Forwarding message to channel " << target << ": " << message << std::endl;
        
        std::string response = ":" + client_state.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
        
        // In a real implementation, you would send this message to all users in the channel
        // This is just a placeholder that echoes back to the sender
        send(client_socket, response.c_str(), response.length(), 0);
    } 
	else 
{
        
        std::cout << "Forwarding message to user " << target << ": " << message << std::endl;
        
        std::string response = ":" + client_state.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
        
        // In a real implementation, you would find the socket of the target user and send the message there
        // This is just a placeholder that echoes back to the sender
        send(client_socket, response.c_str(), response.length(), 0);
    }
}
