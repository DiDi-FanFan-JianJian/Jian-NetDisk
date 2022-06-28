#pragma once
#include <cstring>

constexpr int BLOCK_SIZE = 5;

// ����msg[0]�ж���Ϣ���ͣ���Message(msg + 1)���н���
constexpr int MSG_TYPE_LOGIN = 1;
constexpr int MSG_TYPE_REGISTER = 2;
constexpr int MSG_UPLOAD_FILE = 3;
constexpr int MSG_UPLOAD_BLOCK = 4;

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

// �ļ�
struct UploadFileMessage
{
  UploadFileMessage() {}
  UploadFileMessage(const char* msg) {
    memcpy(this, msg, sizeof(UploadFileMessage));
  }
  char md5[33];  
  int file_size; // �ֽ���
  int block_num; // ǰ�˼����ܿ���
  int block_id;  // ��0��ʼ���
};

// ������Ҫ���ļ���
struct UploadFileResponse
{
  UploadFileResponse() {}
  UploadFileResponse(const char* msg) {
    memcpy(this, msg, sizeof(UploadFileResponse));
  }
  bool checked; // �Ƿ���֤ͨ�� true������һ������
  int block_id;
};

// �ϴ��ļ���
struct UploadBlockMessage
{
  UploadBlockMessage() {}
  UploadBlockMessage(const char* msg) {
    memcpy(this, msg, sizeof(UploadBlockMessage));
  }
  char md5[33];
  int block_id;
  int size; // �ֽ���
  char block_data[BLOCK_SIZE];
};

// �����ϴ����
struct UploadBlockResponse
{
  UploadBlockResponse() {}
  UploadBlockResponse(const char* msg) {
    memcpy(this, msg, sizeof(UploadBlockResponse));
  }
  int next_block_id; // ��һ�����id -1��ʾ�ϴ����
};