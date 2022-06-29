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
  // cout << "parse msg: " << (int)msg[0] << endl;
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
    case MSG_CREATE_DIR:
      return handle_create_dir(msg + 1);
    case MSG_CREATE_FILE_DIR:
      return handle_create_file_dir(msg + 1);
    case MSG_GET_DIR_ID:
      return handle_get_dir_id(msg + 1);
    case MSG_GET_FILE_ID:
      return handle_get_file_id(msg + 1);
    case MSG_GET_DIRS:
      return handle_get_dirs(msg + 1);
    case MSG_GET_FILES:
      return handle_get_files(msg + 1);
    case MSG_MOVE_DIR:
      return handle_move_dir(msg + 1);
    case MSG_MOVE_FILE:
      return handle_move_file(msg + 1);
    case MSG_DELETE_DIR:
      return handle_delete_dir(msg + 1);
    case MSG_DELETE_FILE:
      return handle_delete_file(msg + 1);
    case MSG_GET_FILE_INFO:
      return handle_get_file_info(msg + 1);
    case MSG_DOWNLOAD_BLOCK:
      return handle_download_block(msg + 1);
    case MSG_COPY_FILE:
      return handle_copy_file(msg + 1);
    case MSG_COPY_DIR:
      return handle_copy_dir(msg + 1);
    case MSG_RENAME_FILE:
      return handle_rename_file(msg + 1);
    case MSG_RENAME_DIR:
      return handle_rename_dir(msg + 1);
    default:
      return "unknown type";
  }
}

string router::handle_login(const char* m)
{
  LoginMessage msg(m);
  cout << "handle_login" << endl;
  cout << "username: " << msg.username << endl;
  cout << "password: " << msg.password << endl;
  cout << endl;
  LoginResponse res;
  // 密码md5加密
  auto passwd = get_str_md5(msg.password);
  if (this->db->login(msg.username, passwd))
  {
    res.status = LoginResponse::success;
    auto dir = this->db->get_dir_id(msg.username, "0", "root");
    if (dir == "") 
      res.status = LoginResponse::failed;
    else 
      res.dir = stoi(dir);
  }
  else if (this->db->is_user_exist(msg.username))
  {
    res.status = LoginResponse::passwd_error;
  }
  else
  {
    res.status = LoginResponse::failed;
  }
  return struct_to_string(res);
}

string router::handle_reg(const char* m)
{
  LoginMessage msg(m);
  LoginResponse res;
  auto passwd = get_str_md5(msg.password);
  // 判断用户名是否存在
  if (this->db->is_user_exist(msg.username))
  {
    res.status = LoginResponse::user_exist;
  }
  else if (this->db->reg(msg.username, passwd))
  {
    res.status = LoginResponse::success;
    auto dir = this->db->get_dir_id(msg.username, "0", "root");
    res.dir = stoi(dir);
  }
  else
  {
    res.status = LoginResponse::failed;
  }
  return struct_to_string(res);
}

string router::handle_upload_file(const char* m)
{
  UploadFileMessage msg(m);
  cout << "handle_upload_file" << endl;
  cout << "md5: " << msg.md5 << endl;
  cout << "file_size: " << msg.file_size << endl;
  cout << "block_num: " << msg.block_num << endl;
  cout << "block_id: " << msg.block_id << endl;
  cout << "cur_block: " << this->cur_block[msg.md5] << endl;
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
  res.next_block_id = this->cur_block[msg.md5] + 1;
  fclose(fp);
  return struct_to_string(res);
}

// 上传目录
string router::handle_create_dir(const char* m)
{
  CreateDirMessage msg(m);
  cout << "handle_create_dir" << endl;
  CreateDirResponse res;
  if (this->db->is_dir_exist(to_string(msg.pid), msg.dirname)) {
    res.status = CreateDirResponse::dir_exist;
  }
  else if (this->db->create_dir(msg.username, to_string(msg.pid), msg.dirname)) {
    res.status = CreateDirResponse::success;
  }
  else {
    res.status = CreateDirResponse::failed;
  }
  return struct_to_string(res);
}

// 创建文件目录
string router::handle_create_file_dir(const char* m)
{
  cout << "handle_create_file_dir" << endl;
  CreateFileDirMessage msg(m);
  CreateFileDirResponse res;
  if (this->db->create_file_dir(msg.username, msg.md5, to_string(msg.pid), msg.filename)) {
    res.status = CreateFileDirResponse::success;
  } 
  else {
    res.status = CreateFileDirResponse::failed;
  }
  return struct_to_string(res);
}

// 复制文件
string router::handle_copy_file(const char* m)
{
  cout << "handle_copy_file" << endl;
  CopyFileMessage msg(m);
  CopyFileResponse res;
  if (this->db->copy_file(msg.username, to_string(msg.fid), to_string(msg.pid), msg.filename)) {
    res.status = CopyFileResponse::success;
  }
  else {
    res.status = CopyFileResponse::failed;
  }
  return struct_to_string(res);
}

// 获取目录id
string router::handle_get_dir_id(const char* m)
{
  cout << "handle_get_dir_id" << endl;
  GetDirIDMessage msg(m);
  GetDirIDResponse res;
  string id = this->db->get_dir_id(msg.username, to_string(msg.pid), msg.dirname);
  res.dir = atoi(id.c_str());
  res.status = GetDirIDResponse::success;
  return struct_to_string(res);
}

// 获取文件id
string router::handle_get_file_id(const char* m)
{
  cout << "handle_get_file_id" << endl;
  GetFileIDMessage msg(m);
  GetFileIDResponse res;
  string id = this->db->get_file_id(msg.username, to_string(msg.pid), msg.filename);
  res.dir = atoi(id.c_str());
  res.status = GetFileIDResponse::success;
  return struct_to_string(res);
}

// 获取目录下所有目录
string router::handle_get_dirs(const char* m)
{
  cout << "handle_get_dirs" << endl;
  GetDirsMessage msg(m);
  GetDirsResponse res;
  vector<string> dirs = this->db->get_dirs(msg.username, to_string(msg.pid));
  res.num = dirs.size();
  for (size_t i = 0; i < dirs.size(); ++i)
  {
    strcpy(res.dirname[i], dirs[i].c_str());
  }
  return struct_to_string(res);
}

string router::handle_get_files(const char* m)
{
  cout << "handle_get_files" << endl;
  GetFilesMessage msg(m);
  GetFilesResponse res;
  vector<string> files = this->db->get_files(msg.username, to_string(msg.pid));
  res.num = files.size();
  for (size_t i = 0; i < files.size(); ++i)
  {
    strcpy(res.dirname[i], files[i].c_str());
  }
  return struct_to_string(res);
}

// 移动目录
string router::handle_move_dir(const char* m)
{
  cout << "handle_move_dir" << endl;
  MoveDirMessage msg(m);
  MoveDirResponse res;
  // 获取msg.id对应的目录名
  if (this->db->move_dir(to_string(msg.id), to_string(msg.dst))) {
    res.status = MoveDirResponse::success;
  }
  else {
    res.status = MoveDirResponse::failed;
  }
  return struct_to_string(res);
}

// 移动文件
string router::handle_move_file(const char* m)
{
  cout << "handle_move_file" << endl;
  MoveFileMessage msg(m);
  MoveFileResponse res;
  if (this->db->move_file(to_string(msg.id), to_string(msg.src), to_string(msg.dst))) {
    res.status = MoveFileResponse::success;
  }
  else {
    res.status = MoveFileResponse::failed;
  }
  return struct_to_string(res);
}

// 删除目录
string router::handle_delete_dir(const char* m)
{
  cout << "handle_delete_dir" << endl;
  DeleteDirMessage msg(m);
  DeleteDirResponse res;
  if (this->db->delete_dir(to_string(msg.id))) {
    res.status = DeleteDirResponse::success;
  }
  else {
    res.status = DeleteDirResponse::failed;
  }
  return struct_to_string(res);
}

// 删除文件
string router::handle_delete_file(const char* m)
{
  cout << "handle_delete_file" << endl;
  DeleteFileMessage msg(m);
  DeleteFileResponse res;
  if (this->db->delete_file(to_string(msg.id), to_string(msg.did))) {
    res.status = DeleteFileResponse::success;
  }
  else {
    res.status = DeleteFileResponse::failed;
  }
  return struct_to_string(res);
}

// 获取文件信息 其实就是字节数
string router::handle_get_file_info(const char* m)
{
  cout << "handle_get_file_info" << endl;
  GetFileInfoMessage msg(m);
  GetFileInfoResponse res;
  res.file_size = this->db->get_file_size(to_string(msg.id));
  return struct_to_string(res);
}

// 下载文件块
string router::handle_download_block(const char* m)
{
  cout << "handle_download_file_block" << endl;
  DownloadBlockMessage msg(m);
  DownloadBlockResponse res;
  auto path = this->db->get_file_path(to_string(msg.id));
  int idx = msg.block_id;
  int size = msg.size;
  // 读文件
  FILE* fp = fopen(path.c_str(), "rb");
  if (fp == NULL) {
    cout << "open file failed" << endl;
    exit(1);
  }
  fseek(fp, idx * BLOCK_SIZE, SEEK_SET);
  fread(res.block_data, sizeof(char), size, fp);
  fclose(fp);
  return struct_to_string(res);
}

// 复制目录
string router::handle_copy_dir(const char* m)
{
  cout << "handle_copy_dir" << endl;
  CopyDirMessage msg(m);
  CopyDirResponse res;

  if (this->db->copy_dir(msg.username, to_string(msg.src), to_string(msg.dst))) {
    res.status = CopyDirResponse::success;
  }
  else {
    res.status = CopyDirResponse::failed;
  }
  return struct_to_string(res);
}

// 重命名文件
string router::handle_rename_dir(const char* m)
{
  cout << "handle_rename_dir" << endl;
  RenameDirMessage msg(m);
  RenameDirResponse res;
  if (this->db->rename_dir(to_string(msg.id), msg.newname)) {
    res.status = RenameDirResponse::success;
  }
  else {
    res.status = RenameDirResponse::failed;
  }
  return struct_to_string(res);
}

string router::handle_rename_file(const char* m)
{
  cout << "handle_rename_file" << endl;
  RenameFileMessage msg(m);
  RenameFileResponse res;
  if (this->db->rename_file(to_string(msg.id), to_string(msg.did), msg.newname)) {
    res.status = RenameFileResponse::success;
  }
  else {
    res.status = RenameFileResponse::failed;
  }
  return struct_to_string(res);
}