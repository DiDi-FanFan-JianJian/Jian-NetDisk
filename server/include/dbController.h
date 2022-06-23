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
  // ��ѯ
  bool query(string sql);
  // ����
  bool insert(string sql);
  // ��ӡ���
  void printResult();

public:
  dbController() {}
  dbController(string user, string passwd, string db);
  ~dbController();

  // user
  bool login(string username, string password); // ��¼
  bool reg(string username, string password);   // ע��
  bool is_user_exist(string username);         // �ж��û����Ƿ����
};