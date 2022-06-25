#include "message_parser.h"

template <typename T>
string struct_to_string(T& t)
{
  char buf[sizeof(T)];
  memcpy(buf, &t, sizeof(T));
  return string(buf, sizeof(T));
}

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
      return handle_upload_file(msg + 1);
    case MSG_UPLOAD_BLOCK:
      return handle_upload_block(msg + 1);
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

string router::handle_upload_file(const char* m)
{
  UploadFileMessage msg(m);
  cout << "handle_upload_file" << endl;
  cout << "md5: " << msg.md5 << endl;
  cout << "file_size: " << msg.file_size << endl;
  cout << "block_num: " << msg.block_num << endl;
  cout << "block_id: " << msg.block_id << endl;
  cout << endl;

  // 判断文件是否存在
  if (!this->db->is_file_exist(msg.md5))
  {
    // 创建文件
    this->db->create_file(msg.md5, to_string(msg.file_size));
    this->tot_block[msg.md5] = msg.block_num;
    this->cur_block[msg.md5] = 0;
  }
  if (this->tot_block[msg.md5] == 0) {
    this->tot_block[msg.md5] = msg.block_num;
    this->cur_block[msg.md5] = 0;
  }
  // 判断传输的块是否存在
  if (msg.block_id != this->cur_block[msg.md5] + 1)
  {
    UploadFileResponse res;
    res.checked = false;
    res.block_id = this->cur_block[msg.md5] + 1;
    return struct_to_string(res);
  }
  // 可以传输
  else {
    UploadFileResponse res;
    res.checked = true;
    res.block_id = msg.block_id;
    ++this->cur_block[msg.md5];
    return struct_to_string(res);
  }
}

// 传送数据块
string router::handle_upload_block(const char* m)
{
  UploadBlockMessage msg(m);
  auto path = file_base_path + msg.md5;
  auto write_pose = (msg.block_id - 1) * BLOCK_SIZE;

  cout << "handle_upload_block" << endl;
  cout << "write_pose: " << write_pose << endl;
  cout << "cur_block: " << this->cur_block[msg.md5] << endl;
  cout << "total_block: " << this->tot_block[msg.md5] << endl;
  cout << "block_id: " << msg.block_id << endl;
  cout << "data size: " << msg.size << endl;
  cout << "data: " << msg.block_data << endl;
  cout << endl;

  // 写文件
  FILE* fp = fopen(path.c_str(), "rb+");
  if (fp == nullptr)
  {
    cerr << "open file failed" << endl;
    exit(1);
  }
  // 写入数据
  fseek(fp, write_pose, SEEK_SET);
  fwrite(msg.block_data, sizeof(char), msg.size, fp);
  UploadBlockResponse res;
  res.next_block_id = (this->cur_block[msg.md5] >= this->tot_block[msg.md5]) ? -1 : this->cur_block[msg.md5] + 1;
  fclose(fp);
  return struct_to_string(res);
}