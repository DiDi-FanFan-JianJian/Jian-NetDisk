#include "message_parser.h"

router::router(string user, string passwd, string dbname)
{
  this->db = new dbController(user, passwd, dbname);
}

string router::parse(const char* msg)
{
  int type = msg[0];
  switch (type)
  {
    case MSG_TYPE_LOGIN:
      return handle_login(msg + 1);
    case MSG_TYPE_REGISTER:
      return handle_reg(msg + 1);
    case MSG_UPLOAD_FILE:
      return handle_upload(msg + 1);
    default:
      return "unknown type";
  }
}

string router::handle_login(const char* m)
{
  LoginMessage msg(m);
  if (this->db->login(msg.username, msg.password))
  {
    return "login success";
  }
  else
  {
    return "login failed";
  }
}

string router::handle_reg(const char* m)
{
  LoginMessage msg(m);
  // 判断用户名是否存在
  if (this->db->is_user_exist(msg.username))
  {
    return "user exist";
  }
  if (this->db->reg(msg.username, msg.password))
  {
    return "register success";
  }
  else
  {
    return "register failed";
  }
}

string router::handle_upload(const char* m)
{
  UploadFileMessage msg(m);
  // 判断文件是否存在
  if (!this->db->is_file_exist(msg.md5))
  {
    // 创建文件
  }
}