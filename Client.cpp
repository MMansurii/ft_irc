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

Client::Client() {
    this->port = 0;
    this->ip = "";
    this->username = "";
    this->password = "";
    this->command = "";
    this->file_name = "";
    this->file_path = "";
    this->file_size = "";
    this->file_data = "";
    this->file_data_size = "";
    this->file_data_path = "";
    this->file_data_name = "";
    this->file_data_size_name = "";
    this->file_data_path_name = "";
}

Client::Client(int port, std::string ip) {
    this->port = port;
    this->ip = ip;
    this->username = "";
    this->password = "";
    this->command = "";
    this->file_name = "";
    this->file_path = "";
    this->file_size = "";
    this->file_data = "";
    this->file_data_size = "";
    this->file_data_path = "";
    this->file_data_name = "";
    this->file_data_size_name = "";
    this->file_data_path_name = "";
}

Client::Client(const Client &obj) {
    *this = obj;
}

Client::~Client() {
}

Client &Client::operator=(const Client &obj) {
    this->port = obj.port;
    this->ip = obj.ip;
    this->username = obj.username;
    this->password = obj.password;
    this->command = obj.command;
    this->file_name = obj.file_name;
    this->file_path = obj.file_path;
    this->file_size = obj.file_size;
    this->file_data = obj.file_data;
    this->file_data_size = obj.file_data_size;
    this->file_data_path = obj.file_data_path;
    this->file_data_name = obj.file_data_name;
    this->file_data_size_name = obj.file_data_size_name;
    this->file_data_path_name = obj.file_data_path_name;
    return *this;
}

void Client::setPort(int port) {
    this->port = port;
}

void Client::setIp(std::string ip) {
    this->ip = ip;
}

void Client::setUsername(std::string username) {
    this->username = username;
}

void Client::setPassword(std::string password) {
    this->password = password;
}

void Client::setCommand(std::string command) {
    this->command = command;
}

void Client::setFileName(std::string file_name) {
    this->file_name = file_name;
}

void Client::setFilePath(std::string file_path) {
    this->file_path = file_path;
}

void Client::setFileSize(std::string file_size) {
    this->file_size = file_size;
}

void Client::setFileData(std::string file_data) {
    this->file_data = file_data;
}

void Client::setFileDataSize(std::string file_data_size) {
    this->file_data_size = file_data_size;
}

void Client::setFileDataPath(std::string file_data_path) {
    this->file_data_path = file_data_path;
}

void Client::setFileDataName(std::string file_data_name) {
    this->file_data_name = file_data_name;
}

void Client::setFileDataSizeName(std::string file_data_size_name) {
    this->file_data_size_name = file_data_size_name;
}

void Client::setFileDataPathName(std::string file_data_path_name) {
    this->file_data_path_name = file_data_path_name;
}

int Client::getPort() {
    return this->port;
}

std::string Client::getIp() {
    return this->ip;
}

std::string Client::getUsername() {
    return this->username;
}

std::string Client::getPassword() {
    return this->password;
}

std::string Client::getCommand() {
    return this->command;
}

std::string Client::getFileName() {
    return this->file_name;
}

std::string Client::getFilePath() {
    return this->file_path;
}

std::string Client::getFileSize() {
    return this->file_size;
}

std::string Client::getFileData() {
    return this->file_data;
}

std::string Client::getFileDataSize() {
    return this->file_data_size;
}

std::string Client::getFileDataPath() {
    return this->file_data_path;
}

std::string Client::getFileDataName() {
    return this->file_data_name;
}

std::string Client::getFileDataSizeName() {
    return this->file_data_size_name;
}

std::string Client::getFileDataPathName() {
    return this->file_data_path_name;
}

void Client::createSocket() {
    this->client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->client_socket == -1) {
        std::cerr << "Error: Socket creation failed" << std::endl;
        exit(1);
    }
}

void Client::connectToServer() {
    this->server_address.sin_family = AF_INET;
    this->server_address.sin_port = htons(this->port);
    this->server_address.sin_addr.s_addr = inet_addr(this->ip.c_str());
    if (connect(this->client_socket, (struct sockaddr *)&this->server_address, sizeof(this->server_address)) == -1) {
        std::cerr << "Error: Connection failed" << std::endl;
        exit(1);
    }
}

void Client::sendToServer() {
    if (send(this->client_socket, this->command.c_str(), this->command.size(), 0) == -1) {
        std::cerr << "Error: Sending failed" << std::endl;
        exit(1);
    }
}

void Client::receiveFromServer() {
    if (recv(this->client_socket, this->buffer, MAX_BUFFER, 0) == -1) {
        std::cerr << "Error: Receiving failed" << std::endl;
        exit(1);
    }
}



