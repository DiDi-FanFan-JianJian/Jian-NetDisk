#include "mysocket.h"
#include "message.h"
#include "md5.h"

#include <QDebug>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <string>

using namespace std;

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

// 接收消息
int SJ::MySocket::recvMsg()
{
    return recv(client, recv_buf, sizeof(recv_buf), 0);
}

// 发送用户信息
int SJ::MySocket::sendUserInfo(int type, const std::string &user_name, const std::string &user_password)
{
    if (!is_connected) {
        return -1;
    }
    // 发送消息
    char buf[MAX_BUF_SIZE] = { 0 };
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

// 发送文件
void SJ::MySocket::sendFile(const std::string &file_name)
{
    // 读文件
    FILE *fp = fopen(file_name.c_str(), "r");
    int size = 0;
    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    int block_num = size / BLOCK_SIZE + (size % BLOCK_SIZE ? 1 : 0);
    int cur_idx = 1;
    string md5;
    get_file_md5("./test_file/test.txt", md5);
    char buf_send[this->MAX_BUF_SIZE];
    while (true) {
        getchar();

        cout << "cur_idx: " << cur_idx << endl;
        cout << "block_num: " << block_num << endl;
        cout << "md5: " << md5 << endl;
        cout << endl;

        UploadFileMessage msg;
        strcpy(msg.md5, md5.c_str());
        msg.file_size = size;
        msg.block_num = block_num;
        msg.block_id = cur_idx;

        // 发送文件信息
        buf_send[0] = MSG_UPLOAD_FILE;
        memcpy(buf_send + 1, &msg, sizeof(msg));
        send(client, buf_send, sizeof(msg) + 1, 0);

        cout << "send1" << endl;

        recv(client, buf_send, sizeof(buf_send), 0);
        UploadFileResponse res(buf_send);

        cout << "res.checked " << res.checked << endl;
        cout << "res.block_id " << res.block_id << endl << endl;

        // 不成功就再次发送
        if (!res.checked) {
          cur_idx = res.block_id;
          continue;
        }

        // 读取文件内容
        UploadBlockMessage block_msg;
        memset(block_msg.block_data, 0, BLOCK_SIZE);
        fseek(fp, (cur_idx - 1) * BLOCK_SIZE, SEEK_SET);
        fread(block_msg.block_data, min(BLOCK_SIZE, size - (cur_idx - 1) * BLOCK_SIZE), 1, fp);
        strcpy(block_msg.md5, md5.c_str());
        block_msg.block_id = cur_idx;
        block_msg.size = min(BLOCK_SIZE, size - (cur_idx - 1) * BLOCK_SIZE);

        // 发送文件内容
        memset(buf_send, 0, sizeof(buf_send));
        buf_send[0] = MSG_UPLOAD_BLOCK;
        memcpy(buf_send + 1, &block_msg, sizeof(block_msg));
        send(client, buf_send, sizeof(block_msg) + 1, 0);

        cout << "send2: " << min(BLOCK_SIZE, size - (cur_idx - 1) * BLOCK_SIZE) << endl;

        // 更新当前块索引
        recv(client, buf_send, sizeof(buf_send), 0);
        UploadBlockResponse block_res(buf_send);
        cout << "block_res.next_block_id " << block_res.next_block_id << endl << endl;

        cur_idx = block_res.next_block_id;
        if (cur_idx == -1) {
          break;
        }
    }
}
