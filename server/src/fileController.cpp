#include "dbController.h"

string dbController::get_dir_id(string username, string pid, string dname)
{
  auto uid = get_user_id(username);
  string sql = "select id from dirs where uid = '" + uid + "' and pid = '" + pid + "' and name = '" + dname + "'";
  if (!query(sql))
  {
    return "";
  }
  if (result.size() == 0)
  {
    return "";
  }
  return result[0][0];
}

bool dbController::create_dir(string username, string pid, string dirname)
{
  string uid = get_user_id(username);
  string time = getMysqlTime();
  string sql = "insert into dirs (name, uid, pid, create_time, update_time) values ('" + dirname + "', '" + uid + "', '" + pid + "', '" + time + "', '" + time + "')";
  if (!insert(sql))
  {
    return false;
  }
  return true;
}

bool dbController::is_file_exist(string md5)
{
  string sql = "select * from files where md5 = '" + md5 + "'";
  if (!query(sql))
  {
    return false;
  }
  if ((int)result.size() == 0)
  {
    return false;
  }
  return true;
}

bool dbController::create_file(string md5, string size)
{
  string path = file_base_path + md5;
  // 创建文件
  FILE* fp = fopen(path.c_str(), "wb");
  if (fp == NULL)
  {
    printf("文件创建失败！\n");
    return false;
  }
  // 关闭文件
  fclose(fp);
  string count = "0";
  string sql = "insert into files (md5, size, path, count) values ('" + md5 + "', '" + size + "', '" + path + "', '" + count + "')";
  if (!insert(sql))
  {
    return false;
  }
  return true;
}

string dbController::get_file_id(string md5)
{
  string sql = "select id from files where md5 = '" + md5 + "'";
  if (!query(sql))
  {
    return "";
  }
  if ((int)result.size() == 0)
  {
    return "";
  }
  return result[0][0];
}

string dbController::get_file_id(string username, string pid, string filename)
{
  string uid = get_user_id(username);
  string sql = "select fid from file_dir where uid = '" + uid + "' and did = '" + pid + "' and filename = '" + filename + "'";
  if (!query(sql))
  {
    return "";
  }
  if ((int)result.size() == 0)
  {
    return "";
  }
  return result[0][0];
}

bool dbController::create_file_dir(string username, string md5, string pid, string filename)
{
  string uid = get_user_id(username);
  string fid = get_file_id(md5);
  string did = pid;
  string time = getMysqlTime();
  // fid uid did filename create_time update_time
  string sql = "insert into file_dir (fid, uid, did, filename, create_time, update_time) values ('" + fid + "', '" + uid + "', '" + did + "', '" + filename + "', '" + time + "', '" + time + "')";
  if (!insert(sql))
    return false;
  add_file_link_count(fid);
  return true;
}

bool dbController::copy_file(string username, string fid, string pid, string filename)
{
  string uid = get_user_id(username);
  string did = pid;
  string time = getMysqlTime();
  // fid uid did filename create_time update_time
  string sql = "insert into file_dir (fid, uid, did, filename, create_time, update_time) values ('" + fid + "', '" + uid + "', '" + did + "', '" + filename + "', '" + time + "', '" + time + "')";
  if (!insert(sql))
    return false;
  add_file_link_count(fid);
  return true;
}

vector<string> dbController::get_dirs(string username, string pid)
{
  vector<string> dirs;
  string uid = get_user_id(username);
  string sql = "select name from dirs where uid = '" + uid + "' and pid = '" + pid + "'";
  if (!query(sql))
  {
    return dirs;
  }
  for (auto i = 0; i < result.size(); i++)
  {
    dirs.push_back(result[i][0]);
  }
  // cout << sql << endl;
  // cout << "get_dirs: " << dirs.size() << endl;
  return dirs;
}

vector<string> dbController::get_files(string username, string pid)
{
  vector<string> files;
  string uid = get_user_id(username);
  string sql = "select filename from file_dir where uid = '" + uid + "' and did = '" + pid + "'";
  if (!query(sql))
  {
    return files;
  }
  for (auto i = 0; i < result.size(); i++)
  {
    files.push_back(result[i][0]);
  }
  return files;
}

bool dbController::add_file_link_count(string fid)
{
  string sql = "update files set count = count + 1 where id = '" + fid + "'";
  if (!update(sql))
  {
    return false;
  }
  return true;
}

bool dbController::del_file_link_count(string fid)
{
  string sql = "update files set count = count - 1 where id = '" + fid + "'";
  if (!update(sql))
  {
    return false;
  }
  return true;
}

bool dbController::delete_file(string fid, string did)
{
  string sql = "delete from file_dir where fid = '" + fid + "' and did = '" + did + "'";
  if (!_delete(sql))
  {
    return false;
  }
  if (!del_file_link_count(fid))
  {
    return false;
  }
  return true;
}

bool dbController::delete_dir(string did)
{
  // 递归删除子目录
  string sql = "select id from dirs where pid = '" + did + "'";
  if (!query(sql))
  {
    return false;
  }
  auto tmp = result;
  for (auto i = 0; i < tmp.size(); i++)
  {
    string id = tmp[i][0];
    delete_dir(id);
  }

  // 子文件
  sql = "select fid from file_dir where did = '" + did + "'";
  if (!query(sql))
  {
    return false;
  }
  tmp = result;
  for (auto i = 0; i < tmp.size(); i++)
  {
    string fid = tmp[i][0];
    delete_file(fid, did);
  }

  // 删除目录
  sql = "delete from dirs where id = '" + did + "'";
  if (!_delete(sql))
  {
    return false;
  }
  return true;
}

// 移动目录
bool dbController::move_dir(string did, string dst)
{
  string sql = "update dirs set pid = '" + dst + "' where id = '" + did + "'";
  if (!update(sql))
  {
    return false;
  }
  return true;
}

// 移动文件
bool dbController::move_file(string fid, string src, string dst)
{
  string sql = "update file_dir set did = '" + dst + "' where fid = '" + fid + "' and did = '" + src + "'";
  if (!update(sql))
  {
    return false;
  }
  return true;
}

// 获取文件大小
int dbController::get_file_size(string fid)
{
  string sql = "select size from files where id = '" + fid + "'";
  if (!query(sql))
  {
    return -1;
  }
  if ((int)result.size() == 0)
  {
    return -1;
  }
  return stoi(result[0][0]);
}

// 获取文件路径
string dbController::get_file_path(string fid)
{
  string sql = "select path from files where id = '" + fid + "'";
  if (!query(sql))
  {
    return "";
  }
  if ((int)result.size() == 0)
  {
    return "";
  }
  return result[0][0];
}

// 目录是否存在
bool dbController::is_dir_exist(string pid, string dirname)
{
  string sql = "select id from dirs where pid = '" + pid + "' and name = '" + dirname + "'";
  if (!query(sql))
  {
    return false;
  }
  if ((int)result.size() == 0)
  {
    return false;
  }
  return true;
}

bool dbController::is_file_dir_exist(string pid, string filename)
{
  string sql = "select id from file_dir where did = '" + pid + "' and filename = '" + filename + "'";
  if (!query(sql))
  {
    return false;
  }
  if ((int)result.size() == 0)
  {
    return false;
  }
  return true;
}

// src 起始父目录id dst 目标父目录id
bool dbController::copy_dir(string username, string did, string dst) 
{
  // 获取原目录文件名
  string sql = "select name from dirs where id = '" + did + "'";
  if (!query(sql))
  {
    return false;
  }
  string dname = result[0][0];
  // 创建新目录
  create_dir(username, dst, dname);
  string new_did = get_dir_id(username, dst, dname);
  
  // 递归复制
  sql = "select id from dirs where pid = '" + did + "'";
  if (!query(sql))
  {
    return false;
  }
  auto tmp = result;
  for (auto i = 0; i < tmp.size(); i++)
  {
    string id = tmp[i][0];
    copy_dir(username, id, new_did);
  }

  // 复制文件
  sql = "select fid from file_dir where did = '" + did + "'";
  if (!query(sql))
  {
    return false;
  }
  tmp = result;
  for (auto i = 0; i < tmp.size(); i++)
  {
    string fid = tmp[i][0];
    // 获取文件名 fid did
    sql = "select filename from file_dir where fid = '" + fid + "' and did = '" + did + "'";
    if (!query(sql))
    {
      return false;
    }
    string filename = result[0][0];
    copy_file(username, fid, new_did, filename);
  }
  return true;
}

// 重命名
bool dbController::rename_dir(string did, string newname)
{
  string sql = "update dirs set name = '" + newname + "' where id = '" + did + "'";
  if (!update(sql))
  {
    return false;
  }
  return true;
}

bool dbController::rename_file(string fid, string did, string newname)
{
  string sql = "update file_dir set filename = '" + newname + "' where fid = '" + fid + "' and did = '" + did + "'";
  if (!update(sql))
  {
    return false;
  }
  return true;
}