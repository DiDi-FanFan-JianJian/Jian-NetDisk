#pragma once
#include <cstring>

constexpr int BLOCK_SIZE = 5;

// 根据msg[0]判断消息类型，用Message(msg + 1)进行解析
constexpr int MSG_TYPE_LOGIN = 1;
constexpr int MSG_TYPE_REGISTER = 2;
constexpr int MSG_UPLOAD_FILE = 3;
constexpr int MSG_UPLOAD_BLOCK = 4;

// 注册 登录
struct LoginMessage
{
  LoginMessage() {}
  LoginMessage(const char* msg) {
    memcpy(this, msg, sizeof(LoginMessage));
  }
  char username[20];
  char password[20];
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
  };
  int status;
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
