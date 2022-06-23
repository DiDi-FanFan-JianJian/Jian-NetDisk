#pragma once
#include <cstring>

constexpr int BLOCK_SIZE = 1024 * 16;

// ����msg[0]�ж���Ϣ���ͣ���Message(msg + 1)���н���
constexpr int MSG_TYPE_LOGIN = 1;
constexpr int MSG_TYPE_REGISTER = 2;
constexpr int MSG_UPLOAD_FILE = 3;

// ע�� ��¼
struct LoginMessage
{
  LoginMessage() {}
  LoginMessage(const char* msg) {
    memcpy(this, msg, sizeof(LoginMessage));
  }
  char username[20];
  char password[20];
};

// �ļ�
struct UploadFileMessage
{
  UploadFileMessage() {}
  UploadFileMessage(const char* msg) {
    memcpy(this, msg, sizeof(UploadFileMessage));
  }
  char md5[33];  
  int file_size;
  int block_num;
  int block_id;
};