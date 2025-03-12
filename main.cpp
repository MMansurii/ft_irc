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
#include "src/utils4main.hpp"

#include <iostream>
#include <string>
#include <signal.h>

int cont_run = 1;

int main(int argc, char **argv) {
    if (argc != 3) {
        showError(1, argc, argv);
        return 1;
    }
    if (!checkport(argv[1]))
        return 1;
    if (!check_password(argv[2]))
        return 1;
    try {
        // int cont_run = 1;
        (void)cont_run;
        signal(SIGINT, checkSignal);
        run_server(argc, argv);
    } 
    catch (std::exception &e) {
        stop_server();
        std::cerr << "\033[31mError: " << e.what() << "\033[0m\n";
        return 1;
    }
    return 0;
}


// ./ircserv 6667 mypassword
// git pull --no-rebase origin main