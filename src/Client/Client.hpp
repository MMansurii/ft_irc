

#ifndef client_HPP
#define client_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct sockaddr_in add_sock;

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

        add_sock *Cl_addr;

    public:
        Client();
        ~Client();
        Client(int CL_int_info[3], struct add_sock *Cl_addr);
        // Client(Client const &src);
        // Client &operator=(Client const &src);
        // getters and setters
        int getCl_int_info(int index);
        std::string getCl_str_info(int index);
        // struct add_sock *getCl_addr();
        void setCl_int_info(int index, int value);
        void setCl_str_info(int index, std::string value);
        // void setCl_addr(struct add_sock *Cl_addr);
        void setHostname();

        // Doing some activities on a message in one function
        // Welcome message-> Type 1
        // Reply message-> Type 2
        void do_TMess(std::string Mymessage, int Type);

        // Doing some activities in one function
        // clear-> Type 1
        void do_NoMess(int Type);
       
};

#endif


// struct sockaddr_storage {
//     sa_family_t ss_family;  // Address family (AF_INET or AF_INET6)
//     char __ss_padding[128 - sizeof(sa_family_t)]; // Padding for alignment
//     unsigned long __ss_align;  // Alignment
// };