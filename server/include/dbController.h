#pragma once

#include <string>
#include <iostream>
#pragma once
#include <vector>
#include <mysql/mysql.h>

using namespace std;

const string file_base_path = "./files/";

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
  string get_dir_id(string dname);       // 获取目录id
  bool create_dir(string uid, string pid, string dirname); // 创建目录

  // file
  bool is_file_exist(string md5);  // 判断文件是否存在
  bool create_file(string md5, string size); // 创建文件
  bool upload_file();
};