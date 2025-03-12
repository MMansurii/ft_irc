/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmansuri <mmansuri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 15:28:34 by mmansuri          #+#    #+#             */
/*   Updated: 2025/02/20 19:52:04 by mmansuri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef client_HPP
#define client_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

class Client{
    private:
        // int[3] 0: password, 1: iswelcome, 2: file descriptor
        int Cl_int_info[3];

        struct Cl_str_info{
            std::string username;
            std::string nickname;
            std::string hostname;
            std::string realname;
            std::string server;
            std::string SendingMessage;
        }Cl_str_info;

        struct add_sock *Cl_addr;

    public:
        Client();
        ~Client();
        Client(int CL_int_info[3], struct add_sock *Cl_addr);
        Client(Client const &src);
        Client &operator=(Client const &src);

        int getCl_int_info(int index);
        std::string getCl_str_info(std::string index);
        // struct add_sock *getCl_addr();
        void setCl_int_info(int index, int value);
        void setCl_str_info(std::string index, std::string value);
        // void setCl_addr(struct add_sock *Cl_addr);

        // Doing some activities on a message in one function
        // Welcome message-> Type 1
        // Reply message-> Type 2
        void do_TMess(std::string Mymessage, int Type);

        // Doing some activities in one function
        // clear-> Type 1

        void do_NoMess(int Type);
       
};

#endif