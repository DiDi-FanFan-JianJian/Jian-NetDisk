#pragma once
#include <cstring>

// 根据msg[0]判断消息类型，用Message(msg + 1)进行解析
constexpr int MSG_TYPE_LOGIN = 1;
constexpr int MSG_TYPE_REGISTER = 2;

// 注册 登录
struct LoginMessage
{
  LoginMessage() {}
  LoginMessage(const char* msg) {
    memcpy(this, msg, sizeof(LoginMessage) + 1);
  }
  char username[20];
  char password[20];
};

// 文件