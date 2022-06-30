#include "mysocket.h"
#include "message.h"
#include "global_msg.h"
#include "md5.h"
#include "file_function.h"

#include <QDebug>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

extern Global_Msg g_msg;

SJ::MySocket::MySocket()
{
    client = INVALID_SOCKET;
    is_connected = false;
    has_error = false;
    ip = "1.15.144.212";
    port = 8000;
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

// 获取当前目录下文件
vector<string> SJ::MySocket::get_cur_dirs()
{
    vector<string> ans;
    char buf[MAX_BUF_SIZE] = {0};

    // 获取所有目录
    buf[0] = MSG_GET_DIRS;
    GetDirsMessage msg1;
    strcpy(msg1.username, g_msg.username.c_str());
    msg1.pid = g_msg.get_cur_id();
    memcpy(buf + 1, &msg1, sizeof(msg1));
    send(client, buf, sizeof(msg1) + 1, 0);
    this->recvMsg();
    GetDirsResponse res1(this->recv_buf);
    for (int i = 0; i < res1.num; ++i) {
        // ans.push_back(res1.dirname[i]);
        // 转utf-8
        string utf = g_msg.GbkToUtf8(res1.dirname[i]);
        ans.push_back(utf);
    }

    return ans;
}

vector<string> SJ::MySocket::get_cur_files()
{
    vector<string> ans;
    char buf[MAX_BUF_SIZE] = {0};

    // 获取所有文件
    buf[0] = MSG_GET_FILES;
    GetFilesMessage msg2;
    strcpy(msg2.username, g_msg.username.c_str());
    msg2.pid = g_msg.get_cur_id();
    memcpy(buf + 1, &msg2, sizeof(msg2));
    send(client, buf, sizeof(msg2) + 1, 0);
    this->recvMsg();
    GetFilesResponse res2(this->recv_buf);
    for (int i = 0; i < res2.num; ++i) {
        // ans.push_back(res2.dirname[i]);
        // 转utf-8
        string utf = g_msg.GbkToUtf8(res2.dirname[i]);
        ans.push_back(utf);
    }

    return ans;
}

// 获取指定目录下信息
vector<string> SJ::MySocket::get_cur_dirs(int dir_id)
{
   vector<string> ans;
   char buf[MAX_BUF_SIZE] = {0};
   // 获取所有目录
   buf[0] = MSG_GET_DIRS;
   GetDirsMessage msg1;
   strcpy(msg1.username, g_msg.username.c_str());
   msg1.pid = dir_id;
   memcpy(buf + 1, &msg1, sizeof(msg1));
   send(client, buf, sizeof(msg1) + 1, 0);
   this->recvMsg();
   GetDirsResponse res1(this->recv_buf);
   for (int i = 0; i < res1.num; ++i) {
       // ans.push_back(res1.dirname[i]);
       // 转utf-8
       string utf = g_msg.GbkToUtf8(res1.dirname[i]);
       ans.push_back(utf);
   }
   return ans;
}
vector<string> SJ::MySocket::get_cur_files(int dir_id)
{
   vector<string> ans;
   char buf[MAX_BUF_SIZE] = {0};

   // 获取所有文件
   buf[0] = MSG_GET_FILES;
   GetFilesMessage msg2;
   strcpy(msg2.username, g_msg.username.c_str());
   msg2.pid = dir_id;
   memcpy(buf + 1, &msg2, sizeof(msg2));
   send(client, buf, sizeof(msg2) + 1, 0);
   this->recvMsg();
   GetFilesResponse res2(this->recv_buf);
   for (int i = 0; i < res2.num; ++i) {
       // ans.push_back(res2.dirname[i]);
       // 转utf-8
       string utf = g_msg.GbkToUtf8(res2.dirname[i]);
       ans.push_back(utf);
   }

   return ans;
}
int SJ::MySocket::get_dir_id(string dirname, int dir_id)
{
    string gbk = g_msg.Utf8ToGbk(dirname.c_str());

    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_GET_DIR_ID;
    GetDirIDMessage msg;
    strcpy(msg.username, g_msg.username.c_str());
    strcpy(msg.dirname, gbk.c_str());
    msg.pid = dir_id;
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    GetDirIDResponse res(this->recv_buf);
    return res.dir;
}
int SJ::MySocket::get_file_id(string filename, int dir_id)
{
    string gbk = g_msg.Utf8ToGbk(filename.c_str());

    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_GET_FILE_ID;
    GetFileIDMessage msg;
    strcpy(msg.username, g_msg.username.c_str());
    strcpy(msg.filename, gbk.c_str());
    msg.pid = dir_id;
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    GetFileIDResponse res(this->recv_buf);
    return res.dir;
}


bool SJ::MySocket::create_dir(string dirname)
{
    string gbk = g_msg.Utf8ToGbk(dirname.c_str());

    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_CREATE_DIR;
    CreateDirMessage msg;
    strcpy(msg.username, g_msg.username.c_str());
    strcpy(msg.dirname, gbk.c_str());
    msg.pid = g_msg.get_cur_id();
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    CreateDirResponse res(this->recv_buf);
    return res.status == CreateDirResponse::success;
}

bool SJ::MySocket::create_dir(string dirname, int did)
{
    string gbk = g_msg.Utf8ToGbk(dirname.c_str());

    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_CREATE_DIR;
    CreateDirMessage msg;
    strcpy(msg.username, g_msg.username.c_str());
    strcpy(msg.dirname, gbk.c_str());
    msg.pid = did;
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    CreateDirResponse res(this->recv_buf);
    return res.status == CreateDirResponse::success;
}

bool SJ::MySocket::cd_dir(string dirname)
{
    string gbk = g_msg.Utf8ToGbk(dirname.c_str());

    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_GET_DIR_ID;
    GetDirIDMessage msg;
    strcpy(msg.username, g_msg.username.c_str());
    strcpy(msg.dirname, gbk.c_str());
    msg.pid = g_msg.get_cur_id();
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    GetDirIDResponse res(this->recv_buf);

    g_msg.path.push_back(dirname);
    g_msg.cur_dir.push_back(res.dir);
    return res.status;
}

int SJ::MySocket::get_dir_id(string dirname)
{
    string gbk = g_msg.Utf8ToGbk(dirname.c_str());

    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_GET_DIR_ID;
    GetDirIDMessage msg;
    strcpy(msg.username, g_msg.username.c_str());
    strcpy(msg.dirname, gbk.c_str());
    msg.pid = g_msg.get_cur_id();
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    GetDirIDResponse res(this->recv_buf);
    return res.dir;
}

int SJ::MySocket::get_file_id(string filename)
{
    string gbk = g_msg.Utf8ToGbk(filename.c_str());

    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_GET_FILE_ID;
    GetFileIDMessage msg;
    strcpy(msg.username, g_msg.username.c_str());
    strcpy(msg.filename, gbk.c_str());
    msg.pid = g_msg.get_cur_id();
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    GetFileIDResponse res(this->recv_buf);
    return res.dir;
}

bool SJ::MySocket::move_dir(int id, int src)
{
    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_MOVE_DIR;
    MoveDirMessage msg;
    msg.id = id;
    msg.src = src;
    msg.dst = g_msg.get_cur_id();
    memcpy(buf + 1, &msg, sizeof(MoveDirMessage));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    MoveDirResponse res(this->recv_buf);
    return res.status;
}

bool SJ::MySocket::move_file(int id, int src)
{
    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_MOVE_FILE;
    MoveFileMessage msg;
    msg.id = id;
    msg.src = src;
    msg.dst = g_msg.get_cur_id();
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    MoveFileResponse res(this->recv_buf);
    return res.status;
}

bool SJ::MySocket::copy_dir(int src)
{
    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_COPY_DIR;
    CopyDirMessage msg;
    strcpy(msg.username, g_msg.username.c_str());
    msg.src = src;
    msg.dst = g_msg.get_cur_id();
    memcpy(buf + 1, &msg, sizeof(CopyDirMessage));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    CopyDirResponse res(this->recv_buf);
    return res.status;
}

bool SJ::MySocket::copy_file(int pid, int fid, string filename)
{
    string gbk = g_msg.Utf8ToGbk(filename.c_str());

    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_COPY_FILE;
    CopyFileMessage msg;
    strcpy(msg.username, g_msg.username.c_str());
    msg.pid = pid;
    msg.fid = fid;
    strcpy(msg.filename, gbk.c_str());
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    CopyFileResponse res(this->recv_buf);
    return res.status;
}

bool SJ::MySocket::delete_dir(int id)
{
    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_DELETE_DIR;
    DeleteDirMessage msg;
    msg.id = id;
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    DeleteDirResponse res(this->recv_buf);
    return res.status;
}

bool SJ::MySocket::delete_file(int id, int did)
{
    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_DELETE_FILE;
    DeleteFileMessage msg;
    msg.id = id;
    msg.did = did;
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    DeleteFileResponse res(this->recv_buf);
    return res.status;
}

bool SJ::MySocket::rename_dir(int id, string newname)
{
    string gbk = g_msg.Utf8ToGbk(newname.c_str());

    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_RENAME_DIR;
    RenameDirMessage msg;
    msg.id = id;
    strcpy(msg.newname, gbk.c_str());
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    RenameDirResponse res(this->recv_buf);
    return res.status;
}

bool SJ::MySocket::rename_file(int id, string newname)
{
    string gbk = g_msg.Utf8ToGbk(newname.c_str());

    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_RENAME_FILE;
    RenameFileMessage msg;
    msg.id = id;
    msg.did = g_msg.get_cur_id();
    strcpy(msg.newname, gbk.c_str());
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    RenameFileResponse res(this->recv_buf);
    return res.status;
}

int SJ::MySocket::sendBlock(const std::string &filename)
{
    string md5 = getFileMd5(filename); // 使用utf-8
    char buf[MAX_BUF_SIZE];
    // 初始化文件信息，客户端重启
    if (!g_msg.file_size.count(md5)) {
        g_msg.file_md5[filename] = md5;
        int size = g_msg.file_size[md5] = getFileSize(QString::fromStdString(filename)); // 使用utf-8
        g_msg.block_num[md5] = size / BLOCK_SIZE + (size % BLOCK_SIZE ? 1 : 0);
        g_msg.cur_idx[md5] = 1;
    }

    // 协商获取要传输的块
    int idx = g_msg.cur_idx[md5];
    while (idx <= g_msg.block_num[md5]) {
        UploadFileMessage msg;
        strcpy(msg.md5, g_msg.Utf8ToGbk(md5.c_str()).c_str()); // 使用gbk传给数据库
        msg.file_size = g_msg.file_size[md5];
        msg.block_num = g_msg.block_num[md5];
        msg.block_id = idx;
        
        memset(buf, 0, sizeof(buf));
        buf[0] = MSG_UPLOAD_FILE;
        memcpy(buf + 1, &msg, sizeof(msg));
        send(client, buf, sizeof(msg) + 1, 0);

        this->recvMsg();
        UploadFileResponse res(this->recv_buf);
        if (res.checked)
            break;
        idx = res.block_id;
    }

    // 所有块已经传输完
    g_msg.cur_idx[md5] = idx;
    if (idx > g_msg.block_num[md5]) {
        return 1;
    }

    // 传输一个文件块
    UploadBlockMessage msg;
    memset(msg.block_data, 0, sizeof(msg.block_data));
    FILE* fp = fopen(g_msg.Utf8ToGbk(filename.c_str()).c_str(), "rb"); // 使用gbk打开文件
    int read_size = min(BLOCK_SIZE, g_msg.file_size[md5] - (idx - 1) * BLOCK_SIZE);
    fseek(fp, (idx - 1) * BLOCK_SIZE, SEEK_SET);
    fread(msg.block_data, 1, read_size, fp);
    fclose(fp);
    strcpy(msg.md5, g_msg.Utf8ToGbk(md5.c_str()).c_str());
    msg.block_id = idx;
    msg.size = read_size;

    memset(buf, 0, sizeof(buf));
    buf[0] = MSG_UPLOAD_BLOCK;
    memcpy(buf + 1, &msg, sizeof(msg) + 1);
    send(client, buf, sizeof(msg) + 1, 0);
    
    this->recvMsg();
    UploadBlockResponse res(this->recv_buf);
    g_msg.cur_idx[md5] = res.next_block_id;
    return 1;
}

void SJ::MySocket::init_file_task(const string path, int did)
{
    if (did == -1)
        did = g_msg.get_cur_id();

    cout << path << " " << did << endl;

    // 初始化
    this->sendBlock(path); // utf-8传入参数

    // 在server创建文件结构
    CreateFileDirMessage msg;
    strcpy(msg.username, g_msg.username.c_str());
    msg.pid = did;
    strcpy(msg.md5, g_msg.Utf8ToGbk(getFileMd5(path).c_str()).c_str());
    strcpy(msg.filename, g_msg.Utf8ToGbk(getFileName(path).c_str()).c_str());

    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_CREATE_FILE_DIR;
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);

    this->recvMsg();
    CreateFileDirResponse res(this->recv_buf);

    // 队列
    LoadFileInfo info;
    info.filename = getFileName(path); // 都是utf-8
    info.did = info.fid = 1;
    info.file_size = g_msg.block_num[g_msg.file_md5[path]];
    info.finished_size = info.working = 1;
    info.file_path = path;
    g_msg.upload_file_list.push_back(info);
}


int SJ::MySocket::get_file_size (int fid, int pid){
    char buf[MAX_BUF_SIZE] = {0};
    buf[0] = MSG_GET_FILE_ALL_INFO;
    GetFileAllInfoMessage msg;
    msg.id = fid;
    msg.pid = pid;
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    GetFileAllInfoResponse res(this->recv_buf);
    return res.size;
}
// 添加下载文件：
// filename: 文件名，utf-8
// did: 位于did目录下
// file_path: 文件保存路径，utf-8（最后也是有文件名的，相当于文件完整路径）
void SJ::MySocket::add_download_file(const string &filename, int did, const string &file_path)
{
    createFile(QString::fromStdString(file_path)); // 创建文件
    LoadFileInfo info;
    info.filename = filename;
    info.did = did;
    info.fid = get_file_id(filename, did);
    info.file_size = get_file_size(info.fid, did);
    info.finished_size = 0;
    info.working = WAITING;
    info.file_path = file_path;
    g_msg.download_file_list.push_back(info);
}

// 添加下载文件夹
void SJ::MySocket::add_download_dir(const string &dirname, int did, const string &dir_path)
{
    // 创建文件夹
    createDir(QString::fromStdString(dir_path));
    // 获取选择的文件夹的id
    int pid = get_dir_id(dirname, did);

    // 获取选择的文件夹下的文件列表
    auto list1 = get_cur_files(pid);
    for (auto it: list1) {
        // 合并文件路径
        string path = dir_path + "/" + it;
        // 添加下载文件
        add_download_file(it, pid, path);
    }

    // 获取选择的文件夹下的文件夹列表
    auto list2 = get_cur_dirs(pid);
    for (auto it: list2) {
        // 合并文件路径
        string path = dir_path + "/" + it;
        // 添加下载文件夹
        add_download_dir(it, pid, path);
    }
}

int SJ::MySocket::recvBlock()
{
    // 先找到正在下载的文件
    int working_id = -1;
    int waiting_id = -1;
    for (unsigned i = 0; i < g_msg.download_file_list.size(); i++) {
        if (g_msg.download_file_list[i].working == RUNNING) {
            working_id = i;
            break;
        }
        if (waiting_id < 0 && g_msg.download_file_list[i].working == WAITING) {
            waiting_id = i;
        }
    }
    if (working_id == -1 && waiting_id == -1) {
        return 0;
    }
    if (working_id == -1) {
        working_id = waiting_id;
        g_msg.download_file_list[working_id].working = RUNNING;
    }
    // 下载一个block
    char buf[MAX_BUF_SIZE] = {0};
    DownloadBlockMessage msg;
    msg.id = g_msg.download_file_list[working_id].fid;
    msg.block_id = g_msg.download_file_list[working_id].finished_size / BLOCK_SIZE + 1;
    msg.size = (g_msg.download_file_list[working_id].finished_size + BLOCK_SIZE) > g_msg.download_file_list[working_id].file_size ? g_msg.download_file_list[working_id].file_size - g_msg.download_file_list[working_id].finished_size : BLOCK_SIZE;
    memset(buf, 0, sizeof(buf));
    buf[0] = MSG_DOWNLOAD_BLOCK;
    memcpy(buf + 1, &msg, sizeof(msg));
    send(client, buf, sizeof(msg) + 1, 0);
    this->recvMsg();
    DownloadBlockResponse res(this->recv_buf);
    // 写入文件msg.size个字节
    writeFile(QString::fromStdString(g_msg.download_file_list[working_id].file_path), res.block_data, msg.size, -1);
    // 更新已下载大小
    g_msg.download_file_list[working_id].finished_size += msg.size;
    // 如果下载完成，移除
    if (g_msg.download_file_list[working_id].finished_size == g_msg.download_file_list[working_id].file_size) {
        g_msg.download_file_list.erase(g_msg.download_file_list.begin() + working_id);
    }
    return 1;
}

void SJ::MySocket::init_dir_task(const string path, int did)
{
    auto name = getFileName(path);
    this->create_dir(name, did);
    int new_id = this->get_dir_id(name, did);
    cout << did << " " << new_id << endl;
    QStringList dirList = getDirList(QString::fromStdString(path));
    QStringList fileList = getFileList(QString::fromStdString(path));
    for (auto it: dirList) {
        string name = it.toStdString();
        if (name == "." || name == "..")
            continue;
        init_dir_task(path + "/" + name, new_id);
    }
    for (auto it: fileList) {
        string p = path + "/" + it.toStdString();
        this->init_file_task(p, new_id);
    }
}
