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

viod Client::setHostname(){
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

// int[3] 0: password, 1: iswelcome, 2: file descriptor
Client::Client(int CL_int_info[3], struct add_sock *Cl_addr){
    this->Cl_int_info[0] = 0;
    this->Cl_int_info[1] = 0;
    this->Cl_int_info[2] = CL_int_info[2];
    this->Cl_str_info.server = "IRCserv";
    this->Cl_addr = Cl_addr;
    
    setHostname();
};

// Getters
int Client::getCl_int_info(int index){
    return this->Cl_int_info[index];
};

// struct Cl_str_info{
//     std::string username;
//     std::string nickname;
//     std::string hostname;
//     std::string realname;
//     std::string server;
//     std::string SendingMessage;
// }Cl_str_info;

std::string Client::getCl_str_info(int index){
    if (index == 0)
        return this->Cl_str_info.username;
    else if (index == 1)
        return this->Cl_str_info.nickname;
    else if (index == 2)
        return this->Cl_str_info.hostname;
    else if (index == 3)
        return this->Cl_str_info.realname;
    else if (index == 4)
        return this->Cl_str_info.server;
    else if (index == 5)
        return this->Cl_str_info.SendingMessage;
    else
        return "\033[31m Error: The index for getCl_str_info is out of range \033[0m\n";
};

// Setters
void Client::setCl_int_info(int index, int value){
    if (index == 0)
        this->Cl_int_info[index] = value;
    else if (index == 1)
        this->Cl_int_info[index] = value;
    else if (index == 2)
        std::cerr << "\033[31m Error: The index is incorrect, you can't set file descriptor \033[0m\n";
    else
        std::cerr << "\033[31m Error: The index for setCl_int_info is out of range \033[0m\n";
};

void Client::setCl_str_info(int index, std::string value){
    if (index == 0)
        this->Cl_str_info.username = value;
    else if (index == 1)
        this->Cl_str_info.nickname = value;
    else if (index == 2)
        this->Cl_str_info.hostname = value;
    else if (index == 3)
        this->Cl_str_info.realname = value;
    else if (index == 4)
        std::cerr << "\033[31m Error: The index is incorrect, you can't set server \033[0m\n";
    else if (index == 5)
        this->Cl_str_info.SendingMessage = value;
    else
        std::cerr << "\033[31m Error: The index for setCl_str_info is out of range \033[0m\n";
};


// // Doing some activities on a message in one function
//         // Welcome message-> Type 1
//         // Reply message-> Type 2
//         void do_TMess(std::string Mymessage, int Type);



// struct Cl_str_info{
//     std::string username;0
//     std::string nickname;1
//     std::string hostname;2
//     std::string realname;3
//     std::string server;4
//     std::string SendingMessage;5
// }Cl_str_info;

void Client::do_TMess(std::string Mymessage, int Type)
{
    if (Type == 1)
        {
            // need more works on this
            this->Cl_int_info[1] = 1;
            std::string msg1 = getCl_str_info(1) + " you are welcome to " + getCl_str_info(4) + ", your user name: @" + getCl_str_info(0) + ", host name:" + getCl_str_info(2);
            Client::do_TMess(msg1, 2);
            std::string msg2 =  getCl_str_info(1) + ", your host name is " + getCl_str_info(4) + ". Server created " + Mymessage;
            Client::do_TMess(msg2, 2);
        }
    else if (Type == 2)
        {
            std::cout << "\033[32m " << getCl_str_info(1) << " Send: " << Mymessage << "\033[0m\r\n";
            // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
            Mymessage.append("\r\n");
            size_t len = Mymessage.length();
            int sucseed = 0;
            for (size_t i = 0; i < len; )
            {
                sucseed = send(this->Cl_int_info[2], &(Mymessage[i]),len-i, 0);
                if (sucseed == -1)
                {
                    std::cerr << "\033[31m Error: There is problem with sending reply message by user \033[0m\n";
                    break;
                }
                i=i+sucseed;
            }   
        }
    else
        std::cerr << "\033[31m Error: The Type for Client::do_TMess is out of range \033[0m\n";
};

//         // Doing some activities in one function
//         // clear-> Type 1
//         void do_NoMess(int Type);

void Client::do_NoMess(int Type)
{
    if (Type == 1)
        {
            Cl_str_info.SendingMessage.clear();

        }
    else
        std::cerr << "\033[31m Error: The Type for Client::do_NoMess is out of range \033[0m\n";
};