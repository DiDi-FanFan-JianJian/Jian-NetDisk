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
  // �����ļ�
  FILE* fp = fopen(path.c_str(), "wb");
  if (fp == NULL)
  {
    printf("�ļ�����ʧ�ܣ�\n");
    return false;
  }
  // �ر��ļ�
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

bool dbController::delete_file(string fid)
{
  string sql = "delete from file_dir where fid = '" + fid + "'";
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
  // �ݹ�ɾ����Ŀ¼
  string sql = "select id from dirs where pid = '" + did + "'";
  if (!query(sql))
  {
    return false;
  }
  // ��Ŀ¼
  for (auto i = 0; i < result.size(); i++)
  {
    string id = result[i][0];
    delete_dir(id);
  }
  // ���ļ�
  sql = "select fid from file_dir where did = '" + did + "'";
  if (!query(sql))
  {
    return false;
  }
  for (auto i = 0; i < result.size(); i++)
  {
    string fid = result[i][0];
    delete_file(fid);
  }
  // ɾ��Ŀ¼
  sql = "delete from dirs where id = '" + did + "'";
  if (!_delete(sql))
  {
    return false;
  }
  return true;
}

// �ƶ�Ŀ¼
bool dbController::move_dir(string did, string dst)
{
  string sql = "update dirs set pid = '" + dst + "' where id = '" + did + "'";
  if (!update(sql))
  {
    return false;
  }
  return true;
}

// �ƶ��ļ�
bool dbController::move_file(string fid, string src, string dst)
{
  string sql = "update file_dir set did = '" + dst + "' where fid = '" + fid + "' and did = '" + src + "'";
  if (!update(sql))
  {
    return false;
  }
  return true;
}

// ��ȡ�ļ���С
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

// ��ȡ�ļ�·��
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