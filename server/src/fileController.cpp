#include "dbController.h"

string dbController::get_dir_id(string dname)
{
  string sql = "select id from dirs where name = '" + dname + "'";
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

bool dbController::create_dir(string uid, string pid, string dirname)
{
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
  if (result.size() == 0)
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