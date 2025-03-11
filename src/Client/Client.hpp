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

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>   // for timeval
#include <sys/types.h>  // for fd_set
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#define MAX_CLIENTS 30
#define MAX_BUFFER 1024
#define MAX_FILE_SIZE 1024
#define MAX_FILE_NAME 1024
#define MAX_FILE_PATH 1024
#define MAX_FILE_SIZE 1024
#define MAX_FILE_NAME 1024
#define MAX_FILE_PATH 1024
#define MAX_FILE_SIZE 1024

# ifndef LineSizeAn
#  define LineSizeAn 66
# endif
# ifndef STR1
#  define STR1 "\033[32mDefault constructor called\033[0m\n"
# endif
# ifndef STR2
#  define STR2 "\033[32mCopy constructor called\033[0m\n"
# endif
# ifndef STR3
#  define STR3 "\033[32mDestructor called\033[0m\n"
# endif
# ifndef STR4
#  define STR4 "\033[32mCopy assignment operator called\033[0m\n"
# endif

# ifndef STR6
#  define STR6 "\033[32mConstructor with Number of elements called\033[0m\n"
# endif

class Client{
    private:
        int sockfd;
        int port;
        std::string ip;
        struct sockaddr_in serv_addr;
        char buffer[MAX_BUFFER];
        std::string username;
        std::string password;
        std::string command;
        std::string file_name;
        std::string file_path;
        std::string file_size;
        std::string file_data;
        std::string file_data_size;
        std::string file_data_path;
        std::string file_data_name;
        std::string file_data_size_name;
        std::string file_data_path_name;

    public:
        Client();
        Client(int port, std::string ip);
        Client(const Client &obj);
        ~Client();
        Client &operator=(const Client &obj);
        void setPort(int port);
        void setIp(std::string ip);
        void setUsername(std::string username);
        void setPassword(std::string password);
        void setCommand(std::string command);
        void setFileName(std::string file_name);
        void setFilePath(std::string file_path);
        void setFileSize(std::string file_size);
        void setFileData(std::string file_data);
        void setFileDataSize(std::string file_data_size);
        void setFileDataPath(std::string file_data_path);
        void setFileDataName(std::string file_data_name);
        void setFileDataSizeName(std::string file_data_size_name);
        void setFileDataPathName(std::string file_data_path_name);
        int getPort();
        std::string getIp();
        std::string getUsername();
        std::string getPassword();
        std::string getCommand();
        std::string getFileName();
        std::string getFilePath();
        std::string getFileSize();
        std::string getFileData();
        std::string getFileDataSize();
        std::string getFileDataPath();
        std::string getFileDataName();
        std::string getFileDataSizeName();
        std::string getFileDataPathName();
        void createSocket();
        void connectToServer();
        void sendToServer();
        void receiveFromServer();
        void closeSocket();
        void sendFile();
        void receiveFile();
        void sendFileData();
        void receiveFileData();
        void sendFileDataSize();
        void receiveFileDataSize();
        void sendFileDataPath();
        void receiveFileDataPath();
        void sendFileDataName();
        void receiveFileDataName();
        void sendFileDataSizeName();
        void receiveFileDataSizeName();
        void sendFileDataPathName();
        void receiveFileDataPathName();
};

#endif
