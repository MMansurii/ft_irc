/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmansuri <mmansuri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 15:28:34 by mmansuri          #+#    #+#             */
/*   Updated: 2025/02/20 19:52:04 by mmansuri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

int main(int argc, char **argv) {
    Client client;
    client.setPort(8080);
    client.setIp("
    client.setUsername("mmansuri");
    client.setPassword("password");
    client.setCommand("ls");
    client.setFileName("file.txt");
    client.setFilePath("/Users/mmansuri/Desktop/");
    client.setFileSize("1024");
    client.setFileData("Hello World");
    client.setFileDataSize("1024");

    std::cout << "Port: " << client.getPort() << std::endl;

    std::cout << "IP: " << client.getIp() << std::endl;
    std::cout << "Username: " << client.getUsername() << std::endl;
    std::cout << "Password: " << client.getPassword() << std::endl;
    std::cout << "Command: " << client.getCommand() << std::endl;
    std::cout << "File Name: " << client.getFileName() << std::endl;
    std::cout << "File Path: " << client.getFilePath() << std::endl;
    std::cout << "File Size: " << client.getFileSize() << std::endl;

    return 0;
}