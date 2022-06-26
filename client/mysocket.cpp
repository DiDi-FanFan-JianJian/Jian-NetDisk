#include "mysocket.h"
#include "message.h"

#include <QDebug>
#include <iostream>
#include <fstream>
#include <errno.h>

SJ::MySocket::MySocket()
{
    client = INVALID_SOCKET;
    is_connected = false;
    has_error = false;
    ip = "1.15.144.212";
    port = 8000;

    // 连接到服务端
//    if (getConnect() == -1) {
//        has_error = true;
//    }
//    else {
//        is_connected = true;
//    }
}

SJ::MySocket::MySocket(std::string ip, int port)
{
    client = INVALID_SOCKET;
    is_connected = false;
    has_error = false;
    this->ip = ip;
    this->port = port;

    // 连接到服务端
    if (getConnect() == -1) {
        has_error = true;
    }
    else {
        is_connected = true;
    }
}

SJ::MySocket::~MySocket()
{
    if (is_connected) {
        disConnect();
    }
}

int SJ::MySocket::getConnect()
{
    // 指定Windows Sockets版本
    int version = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(version, &wsaData) != 0) {
        std::cout << "WSAStartup failed" << std::endl;
        has_error = true;
        return -1;
    }
    // 创建套接字
    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == INVALID_SOCKET) {
        std::cout << "socket error" << std::endl;
        has_error = true;
        return -1;
    }
    // 连接服务端
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    if (connect(client, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cout << "connect error" << std::endl;
        has_error = true;
        closesocket(client);
        return -1;
    }
    std::cout << "connect success" << std::endl;
    is_connected = true;
    return 0;
}

int SJ::MySocket::disConnect()
{
    if (is_connected) {
        closesocket(client);
        is_connected = false;
    }
    return 0;
}

int SJ::MySocket::reConnect()
{
    if (is_connected) {
        disConnect();
    }
    while (!is_connected) {
        if (getConnect() == 0) {
            is_connected = true;
            return 0;
        }
        Sleep(1000);
    }
    return -1;
}

bool SJ::MySocket::isConnected()
{
    return is_connected;
}

bool SJ::MySocket::hasError()
{
    return has_error;
}

void SJ::MySocket::printError()
{
    if (has_error) {
        std::cout << "Error: " << WSAGetLastError() << std::endl;
    }
}

// 发送用户信息
int SJ::MySocket::sendUserInfo(int type, const std::string &user_name, const std::string &user_password)
{
    if (!is_connected) {
        return -1;
    }
    // 发送消息
    char buf[1024] = { 0 };
    LoginMessage msg;
    buf[0] = type;
    strcpy(msg.username, user_name.c_str());
    strcpy(msg.password, user_password.c_str());
    memcpy(buf + 1, &msg, sizeof(msg));
    int len = sizeof(msg) + 1;
    int ret = send(client, buf, len, 0);
    if (ret == SOCKET_ERROR) {
        std::cout << "send error" << std::endl;
        has_error = true;
        return -1;
    }
    return 0;
}
