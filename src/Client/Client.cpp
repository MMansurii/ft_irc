
#include "Client.hpp"
#include <iostream>
#include <string>
#include <netdb.h> 

Client::Client(){

};

Client::~Client(){

};

void Client::setHostname(){
    // Retrieve peer address and convert to numeric hostname
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    int sockfd = this->getUserFd();
    if (getpeername(sockfd, reinterpret_cast<sockaddr*>(&addr), &len) != 0) {
        this->Cl_str_info.hostname = "unknown";
        return;
    }
    char hostbuf[NI_MAXHOST];
    int err = getnameinfo(reinterpret_cast<sockaddr*>(&addr), len,
                          hostbuf, sizeof(hostbuf),
                          NULL, 0, NI_NUMERICHOST);
    if (err != 0) {
        this->Cl_str_info.hostname = "unknown";
    } else {
        this->Cl_str_info.hostname = hostbuf;
    }
};

// int[3] 0: password, 1: iswelcome, 2: file descriptor
// Construct client from socket descriptor
Client::Client(int socketFd) {
    // 0: PASS flag, 1: welcome flag, 2: socket descriptor
    Cl_int_info[0] = 0;
    Cl_int_info[1] = 0;
    Cl_int_info[2] = socketFd;
    Cl_str_info.server = "IRCserv";
}

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
            Client::setCl_int_info(1, 1);
            // this->Cl_int_info[1] = 1;
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
// Retrieve underlying socket descriptor
int Client::getUserFd() const {
    return Cl_int_info[2];
}

// Clear message buffer
void Client::clearMessage() {
    Cl_str_info.SendingMessage.clear();
}

// Set message buffer
void Client::setMessage(const std::string& msg) {
    Cl_str_info.SendingMessage = msg;
}

// Get message buffer
std::string Client::getMessage() const {
    return Cl_str_info.SendingMessage;
}

// Get client nickname
std::string Client::getNickname() const {
    return Cl_str_info.nickname;
}

// Send reply to client
void Client::sendReply(const std::string& reply) {
    do_TMess(reply, 2);
}