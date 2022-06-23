#pragma once

#include <string>
#include <iostream>
#pragma once
#include <vector>
#include <mysql/mysql.h>

using namespace std;

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

public:
  dbController() {}
  dbController(string user, string passwd, string db);
  ~dbController();

  // user
  bool login(string username, string password); // 登录
  bool reg(string username, string password);   // 注册
  bool is_user_exist(string username);         // 判断用户名是否存在
};