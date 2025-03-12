/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmansuri <mmansuri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 15:28:34 by mmansuri          #+#    #+#             */
/*   Updated: 2025/03/11 17:23:43 by ftapponn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "src/Client/Client.hpp"

void showError(int i, int ac,char **av) {
    if (i == 1)
    std::cerr << "\033[31mError: Usage: " << av[0] << " <server> <port>\033[0m\n";
    else if (i == 2)
        std::cerr << "\033[31mError: Connection failed\033[0m\n";
    else if (i == 3)
        std::cerr << "\033[31mError: Connection lost\033[0m\n";
    else if (i == 4)
        std::cerr << "\033[31mError: Invalid command\033[0m\n";
    else if (i == 5)
        std::cerr << "\033[31mError: Invalid arguments\033[0m\n";
    else if (i == 6)
        std::cerr << "\033[31mError: Invalid file\033[0m\n";
    else if (i == 7)
        std::cerr << "\033[31mError: File not found\033[0m\n";
    else if (i == 8)
        std::cerr << "\033[31mError: File already exists\033[0m\n";
}


int checkport_digit(std::string port) {
    if (port.find_first_not_of("0123456789") != std::string::npos) {
        std::cerr << "\033[31mError: Port must be a number\033[0m\n";
        return 0;
    }
    return 1;   
}

int checkport_range(std::string port) {
    int port_int = std::stoi(port);
    if (port_int < 0 || port_int > 65535) {
        std::cerr << "\033[31mError: Port must be between 0 and 65535\033[0m\n";
        return 0;
    }
    return 1;
}

int checkport_reserved(std::string port) {
    int port_int = std::stoi(port);
    if (port_int < 1024) {
        std::cerr << "\033[31mError: Port must be above 1024\033[0m\n";
        return 0;
    }
    return 1;
}

int checkport_length(std::string port) {
    if (port.length() > 5) {
        std::cerr << "\033[31mError: Port must be less than 5 digits\033[0m\n";
        return 0;
    }
    return 1;
}



int checkport(std::string port) {
    if (!checkport_digit(port))
        return 0;
    if (!checkport_range(port))
        return 0;
    if (!checkport_reserved(port))
        return 0;
    if (!checkport_length(port))
        return 0;
    return 1;
}

extern int cont_run;

void chechSignal(int signum) {
    (void)signum;
    cont_run = 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        showError(1, argc, argv);
        return 1;
    }
    try {
        int cont_run = 1;
        signal(SIGINT, chechSignal);
        run_server(int argc, char **argv);
    } 
    catch (std::exception &e) {
        stop_server();
        std::cerr << "\033[31mError: " << e.what() << "\033[0m\n";
        return 1;
    }
    return 0;
}
