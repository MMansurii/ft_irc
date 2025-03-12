

#ifndef UTILS4MAIN_HPP
#define UTILS4MAIN_HPP

#include <iostream>
#include <string>

void showError(int i, int ac, char **av);
int checkport_digit(std::string port);
int checkport_range(std::string port);
int checkport_reserved(std::string port);
int checkport_length(std::string port);
int checkport_leading_zeros(std::string port);
int checkport(std::string port);
int check_password(std::string password);
void chechSignal(int signal);

void run_server(int argc, char **argv);
void stop_server(); 



#endif