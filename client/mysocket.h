#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <ws2tcpip.h>
#include <Winsock2.h>
#include <string>

namespace SJ
{

    class MySocket
    {
    public:
        static const int MAX_BUF_SIZE = 1024 * 4;
        char recv_buf[MAX_BUF_SIZE];

    public:
        MySocket();
        MySocket(std::string ip, int port);
        ~MySocket();
        // �������
        int getConnect();
        int disConnect();
        int reConnect();
        bool isConnected();
        bool hasError();
        void printError();

        // �û��˺ţ���¼ע�ᣩ
        int sendUserInfo(int type, const std::string &user_name, const std::string &user_password);
        int getSignin(std::string &user_name, std::string &user_password);
        int getSignUp(std::string &user_name, std::string &user_password);

        // �ļ����䣨�ϴ����أ�
        int sendFileInfo(const std::string &file_name, const std::string &file_size);
        int recvFileInfo(std::string &file_name, std::string &file_size);
        void sendFile(const std::string &file_name);
        void recvFile(const std::string &file_name);
        int sendBlock(const std::string &file_name, int block_id);
        int recvBlock(const std::string &file_name, int block_id);

        // �������ƶ������ơ���������ɾ����
        int sendMsg(const std::string &message);
        int recvMsg(const std::string &message);
        int recvMsg();

    private:
        SOCKET client;
        std::string ip;
        int port;
        bool is_connected;
        bool has_error;

        std::string msg2string();
        void string2msg(std::string msg);
    };

}

#endif // MYSOCKET_H
