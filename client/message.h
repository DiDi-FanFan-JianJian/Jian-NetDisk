#pragma once
#include <cstring>
#include <string>
using namespace std;

constexpr int BLOCK_SIZE = 5;
constexpr int NAME_SIZE = 20;
constexpr int FILE_NUM = 50; // 文件夹下文件最大数量

// 根据msg[0]判断消息类型，用Message(msg + 1)进行解析
constexpr int MSG_TYPE_LOGIN = 1;
constexpr int MSG_TYPE_REGISTER = 2;
constexpr int MSG_UPLOAD_FILE = 3;
constexpr int MSG_UPLOAD_BLOCK = 4;
constexpr int MSG_CREATE_DIR = 5;
constexpr int MSG_CREATE_FILE_DIR = 6;
constexpr int MSG_GET_DIR_ID = 7;
constexpr int MSG_GET_FILE_ID = 8;
constexpr int MSG_GET_DIRS = 9;
constexpr int MSG_GET_FILES = 10;
constexpr int MSG_MOVE_DIR = 11;
constexpr int MSG_MOVE_FILE = 12;
constexpr int MSG_DELETE_DIR = 13;
constexpr int MSG_DELETE_FILE = 14;
constexpr int MSG_GET_FILE_INFO = 15;
constexpr int MSG_DOWNLOAD_BLOCK = 16;

// 注册 登录
struct LoginMessage
{
  LoginMessage() {}
  LoginMessage(const char* msg) {
    memcpy(this, msg, sizeof(LoginMessage));
  }
  char username[NAME_SIZE];
  char password[NAME_SIZE];
};

struct LoginResponse
{
  LoginResponse() {}
  LoginResponse(const char* msg) {
    memcpy(this, msg, sizeof(LoginResponse));
  }
  enum flags{
    success = 0,
    failed = 1,
    user_exist = 2,
    passwd_error = 3
  };
  int status;
  int dir; // 根目录编号
};

// 文件
struct UploadFileMessage
{
  UploadFileMessage() {}
  UploadFileMessage(const char* msg) {
    memcpy(this, msg, sizeof(UploadFileMessage));
  }
  char md5[33];
  int file_size; // 字节数
  int block_num; // 前端计算总块数
  int block_id;  // 从0开始编号
};

// 反馈需要的文件块
struct UploadFileResponse
{
  UploadFileResponse() {}
  UploadFileResponse(const char* msg) {
    memcpy(this, msg, sizeof(UploadFileResponse));
  }
  bool checked; // 是否验证通过 true才能下一步传输
  int block_id;
};

// 上传文件块
struct UploadBlockMessage
{
  UploadBlockMessage() {}
  UploadBlockMessage(const char* msg) {
    memcpy(this, msg, sizeof(UploadBlockMessage));
  }
  char md5[33];
  int block_id;
  int size; // 字节数
  char block_data[BLOCK_SIZE];
};

// 反馈上传情况
struct UploadBlockResponse
{
  UploadBlockResponse() {}
  UploadBlockResponse(const char* msg) {
    memcpy(this, msg, sizeof(UploadBlockResponse));
  }
  int next_block_id; // 下一个块的id -1表示上传完成
};

// 创建目录
struct CreateDirMessage
{
  CreateDirMessage() {}
  CreateDirMessage(const char* msg) {
    memcpy(this, msg, sizeof(CreateDirMessage));
  }
  char username[NAME_SIZE];
  char dirname[NAME_SIZE];
  int pid; // 父目录编号
};

typedef LoginResponse CreateDirResponse;

// 获取目录id
struct GetDirIDMessage
{
  GetDirIDMessage() {}
  GetDirIDMessage(const char* msg) {
    memcpy(this, msg, sizeof(GetDirIDMessage));
  }
  char username[NAME_SIZE];
  char dirname[NAME_SIZE];
  int pid; // 父目录编号
};

typedef LoginResponse GetDirIDResponse;

// 获取文件id
struct GetFileIDMessage
{
  GetFileIDMessage() {}
  GetFileIDMessage(const char* msg) {
    memcpy(this, msg, sizeof(GetFileIDMessage));
  }
  char username[NAME_SIZE];
  char filename[NAME_SIZE];
  int pid; // 父目录编号
};

typedef LoginResponse GetFileIDResponse;

// 创建文件目录
struct CreateFileDirMessage
{
  CreateFileDirMessage() {}
  CreateFileDirMessage(const char* msg) {
    memcpy(this, msg, sizeof(CreateFileDirMessage));
  }
  char username[NAME_SIZE];
  int pid; // 父目录编号
  char md5[33]; // 文件的md5
  char filename[NAME_SIZE];
};

typedef LoginResponse CreateFileDirResponse;

// 获取文件夹下目录
struct GetDirsMessage
{
  GetDirsMessage() {}
  GetDirsMessage(const char* msg) {
    memcpy(this, msg, sizeof(GetDirsMessage));
  }
  char username[NAME_SIZE];
  int pid; // 父目录编号
};

struct GetDirsResponse
{
  GetDirsResponse() {}
  GetDirsResponse(const char* msg) {
    memcpy(this, msg, sizeof(GetDirsResponse));
  }
  int num; // 目录数量
  char dirname[FILE_NUM][NAME_SIZE]; // 目录名
};

// 获取文件夹下文件
typedef GetDirsMessage GetFilesMessage;
typedef GetDirsResponse GetFilesResponse;

// 更改文件夹位置
struct MoveDirMessage
{
  MoveDirMessage() {}
  MoveDirMessage(const char* msg) {
    memcpy(this, msg, sizeof(MoveDirMessage));
  }
  int id; // 目录编号
  int src; // 源父目录编号
  int dst; // 目标父目录编号
};

typedef LoginResponse MoveDirResponse;

typedef MoveDirMessage MoveFileMessage;
typedef MoveDirResponse MoveFileResponse;

// 删除目录
struct DeleteDirMessage
{
  DeleteDirMessage() {}
  DeleteDirMessage(const char* msg) {
    memcpy(this, msg, sizeof(DeleteDirMessage));
  }
  int id; // 目录编号
};

typedef LoginResponse DeleteDirResponse;

typedef DeleteDirMessage DeleteFileMessage;
typedef DeleteDirResponse DeleteFileResponse;


// 获取文件信息
struct GetFileInfoMessage
{
  GetFileInfoMessage() {}
  GetFileInfoMessage(const char* msg) {
    memcpy(this, msg, sizeof(GetFileInfoMessage));
  }
  int id; // 文件编号
};

struct GetFileInfoResponse
{
  GetFileInfoResponse() {}
  GetFileInfoResponse(const char* msg) {
    memcpy(this, msg, sizeof(GetFileInfoResponse));
  }
  int file_size; // 字节数
};


// 下载文件块
struct DownloadBlockMessage
{
  DownloadBlockMessage() {}
  DownloadBlockMessage(const char* msg) {
    memcpy(this, msg, sizeof(DownloadBlockMessage));
  }
  int id;
  int block_id; // 块索引 从1开始编号
  int size; // 字节数
};

struct DownloadBlockResponse
{
  DownloadBlockResponse() {}
  DownloadBlockResponse(const char* msg) {
    memcpy(this, msg, sizeof(DownloadBlockResponse));
  }
  char block_data[BLOCK_SIZE];
};
