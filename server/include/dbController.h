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
  // ��ѯ
  bool query(string sql);
  // ����
  bool insert(string sql);
  // ��ӡ���
  void printResult();
  // ��ȡmysqlʱ���
  string getMysqlTime();
  // ��ȡ��һ������id
  string getNextId(string table);

public:
  dbController() {}
  dbController(string user, string passwd, string db);
  ~dbController();

  // user
  bool login(string username, string password); // ��¼
  bool reg(string username, string password);   // ע��
  bool is_user_exist(string username);         // �ж��û����Ƿ����
  string get_user_id(string username);         // ��ȡ�û�id

  // dir
  string get_dir_id(string dname);       // ��ȡĿ¼id
  bool create_dir(string uid, string pid, string dirname); // ����Ŀ¼

  // file
  bool is_file_exist(string md5);  // �ж��ļ��Ƿ����
  bool create_file(string md5, string size); // �����ļ�
  bool upload_file();
};