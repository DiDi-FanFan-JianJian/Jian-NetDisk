#pragma once

#include <string>
#include <iostream>
#pragma once
#include <vector>
#include <mysql/mysql.h>

using namespace std;

const string file_base_path = "/home/netdisk/files/";

class dbController {
private:
  MYSQL* conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  vector<vector<string> > result;

private:
  // 查询
  bool query(string sql);
  // 插入
  bool insert(string sql);
  // 更新
  bool update(string sql);
  // 删除
  bool _delete(string sql);
  // 打印结果
  void printResult();
  // 获取mysql时间戳
  string getMysqlTime();
  // 获取下一个自增id
  string getNextId(string table);

public:
  dbController() {}
  dbController(string user, string passwd, string db);
  ~dbController();

  // user
  bool login(string username, string password); // 登录
  bool reg(string username, string password);   // 注册
  bool is_user_exist(string username);         // 判断用户名是否存在
  string get_user_id(string username);         // 获取用户id

  // dir
  string get_dir_id(string username, string pid, string dname);       // 获取目录id
  bool create_dir(string username, string pid, string dirname); // 创建目录
  vector<string> get_dirs(string username, string pid); // 获取目录下所有子目录
  bool delete_dir(string did); // 删除目录
  bool move_dir(string did, string dst); // 移动目录
  bool is_dir_exist(string pid, string dirname); // 判断目录是否存在
  bool copy_dir(string username, string did, string dst); // 复制目录
  bool rename_dir(string did, string newname); // 重命名目录

  // file
  string get_file_id(string md5);       // 获取文件id
  string get_file_id(string username, string pid, string filename);
  bool add_file_link_count(string fid); // 增加链接计数
  bool del_file_link_count(string fid); // 减少链接计数
  bool is_file_exist(string md5);       // 判断文件是否存在
  bool create_file(string md5, string size); // 创建文件
  bool upload_file();
  vector<string> get_files(string username, string pid); // 获取目录下所有文件
  bool create_file_dir(string username, string md5, string pid, string filename); // 创建文件目录
  bool delete_file(string fid, string did); // 删除文件
  bool move_file(string fid, string src, string dst); // 移动文件
  int get_file_size(string fid); // 获取文件大小
  string get_file_path(string fid); // 获取文件路径
  bool is_file_dir_exist(string pid, string filename); // 判断文件目录是否存在
  bool copy_file(string username, string fid, string pid, string filename); // 文件复制
  bool rename_file(string fid, string did, string newname); // 重命名文件
};