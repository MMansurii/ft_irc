// Function: Utility functions for main.cpp

#include "utils4main.hpp"

// show error message
void showError(int i, int ac,char **av) {
    (void)ac;
    if (i == 1)
    std::cerr << "\033[31mError: Usage: " << av[0] << " <server> <port>\033[0m\n";
    else if (i == 2)
        std::cerr << "\033[31mError: Connection failed\033[0m\n";
}

// check port number
int checkport_digit(std::string port) {
    if (port.find_first_not_of("0123456789") != std::string::npos) {
        std::cerr << "\033[31mError: Port must be a number\033[0m\n";
        return 0;
    }
    return 1;   
}

int checkport_range(std::string port) {
    int port_int = std::stoi(port.c_str());
    // 65535 is the maximum port number
    if (port_int < 0 || port_int > 65535) {
        std::cerr << "\033[31mError: Port must be between 0 and 65535\033[0m\n";
        return 0;
    }
    return 1;
}

int checkport_reserved(std::string port) {
    int port_int = std::stoi(port.c_str());
    // 1024 is the minimum port number
    // 0-1023 are reserved for system use
    if (port_int < 1024) {
        std::cerr << "\033[31mError: Port must be above 1024\033[0m\n";
        return 0;
    }
    return 1;
}

int checkport_length(std::string port) {
    if (port.length() > 5) {
        std::cerr << "\033[31mError: Port must be equal or less than 5 digits\033[0m\n";
        return 0;
    }
    return 1;
}

// Ensure no leading zeros (e.g., "0080" should be invalid)
int checkport_leading_zeros(std::string port) {
    if (port.length() > 1 && port[0] == '0') {
        std::cerr << "\033[31mError: Port must not have leading zeros\033[0m\n";
        return 0;
    }
    return 1;
}

int checkport(std::string port) {
    if (!port.length()) {
        std::cerr << "\033[31mError: Port must be a number\033[0m\n";
        return 0;
    }
    if (!checkport_digit(port))
        return 0;
    if (!checkport_range(port))
        return 0;
    if (!checkport_reserved(port))
        return 0;
    if (!checkport_length(port))
        return 0;
    if (!checkport_leading_zeros(port))
        return 0;
    return 1;
}
 // check password
int check_password(std::string password) {
    if (password.empty()) {
        std::cerr << "\033[31mError: Password must not be empty\033[0m\n";
        return 0;
    }
    if (password.length() < 8) {
        std::cerr << "\033[31mError: Password must be at least 8 characters long\033[0m\n";
        return 0;
    }
    return 1;
}


void run_server(int argc, char **argv) {
    (void)argc;
    (void)argv;
    // Run server
}

void stop_server() {
    // Stop server
}

