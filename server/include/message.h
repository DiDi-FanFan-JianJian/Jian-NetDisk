#pragma once
#include <cstring>

// ����msg[0]�ж���Ϣ���ͣ���Message(msg + 1)���н���
constexpr int MSG_TYPE_LOGIN = 1;
constexpr int MSG_TYPE_REGISTER = 2;

// ע�� ��¼
struct LoginMessage
{
  LoginMessage() {}
  LoginMessage(const char* msg) {
    memcpy(this, msg, sizeof(LoginMessage) + 1);
  }
  char username[20];
  char password[20];
};

// �ļ�