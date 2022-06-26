#ifndef MYSOCKET_H
#define MYSOCKET_H

#include "message.h"

#include <ws2tcpip.h>
#include <Winsock2.h>
#include <string>
#include <iostream>
#include <QDebug>
#include <fstream>
#include <errno.h>

namespace SJ
{

    class MySocket
    {
    public:
        MySocket();
        MySocket(std::string ip, int port);
        ~MySocket();
        // 连接相关
        int getConnect();
        int disConnect();
        int reConnect();
        bool isConnected();
        bool hasError();
        void printError();

        // 发送接收相关
        int sendUserInfo(const std::string &user_name, const std::string &user_ip);

        int sendFileInfo(const std::string &file_name, const std::string &file_size);
        int recvFileInfo(std::string &file_name, std::string &file_size);

        int sendMsg(const std::string &message);
        int recvMsg(const std::string &message);

    private:
        SOCKET client;
        std::string ip;
        int port;
        bool is_connected;
        bool has_error;

        std::string msg2string();
        void string2msg(std::string msg);
    };

    /* -------------- *
     *    SignUpReq
     *    SignInReq
     *----------------*/
    struct SignUpReq
    {
        std::string username;
        std::string password;
        std::string session;
    };
    typedef SignUpReq SignInReq;

    /* -------------- *
     *    SignUpRes
     *    SignInRes
     *----------------*/
    struct SignUpRes
    {
        int code;
        std::string message;
        std::string session;
    };
    typedef SignUpRes SignInRes;

}

#endif // MYSOCKET_H
