/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmansuri <mmansuri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 15:28:34 by mmansuri          #+#    #+#             */
/*   Updated: 2025/02/20 19:52:04 by mmansuri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <iostream>
#include <string>
#include <netdb.h> 

Client::Client(){

};

Client::~Client(){

};

// int[3] 0: password, 1: iswelcome, 2: file descriptor
Client::Client(int CL_int_info[3], struct add_sock *Cl_addr){
    this->Cl_int_info[0] = 0;
    this->Cl_int_info[1] = 0;
    this->Cl_int_info[2] = CL_int_info[2];
    this->Cl_str_info.server = "IRCserv";
    this->Cl_addr = Cl_addr;
    
    int sucseed1;
    char arrayhost[1025];
    struct sockaddr_in *addr = reinterpret_cast<struct sockaddr_in *>(Cl_addr);
    sucseed1 = getnameinfo(reinterpret_cast<sockaddr *>(addr), sizeof(sockaddr_in), arrayhost, sizeof(arrayhost),NULL, 0, 0);
    if (sucseed1 != 0)
    {
        std::cerr << "\033[31m Error: There is problem with Host name \033[0m\n";
        this->Cl_str_info.hostname = "Unrecognized";
    }
    this->Cl_str_info.hostname = arrayhost;
    
};