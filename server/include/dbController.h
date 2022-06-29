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
  // ��ѯ
  bool query(string sql);
  // ����
  bool insert(string sql);
  // ����
  bool update(string sql);
  // ɾ��
  bool _delete(string sql);
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
  string get_dir_id(string username, string pid, string dname);       // ��ȡĿ¼id
  bool create_dir(string username, string pid, string dirname); // ����Ŀ¼
  vector<string> get_dirs(string username, string pid); // ��ȡĿ¼��������Ŀ¼
  bool delete_dir(string did); // ɾ��Ŀ¼
  bool move_dir(string did, string dst); // �ƶ�Ŀ¼
  bool is_dir_exist(string pid, string dirname); // �ж�Ŀ¼�Ƿ����
  bool copy_dir(string username, string did, string dst); // ����Ŀ¼
  bool rename_dir(string did, string newname); // ������Ŀ¼

  // file
  string get_file_id(string md5);       // ��ȡ�ļ�id
  string get_file_id(string username, string pid, string filename);
  bool add_file_link_count(string fid); // �������Ӽ���
  bool del_file_link_count(string fid); // �������Ӽ���
  bool is_file_exist(string md5);       // �ж��ļ��Ƿ����
  bool create_file(string md5, string size); // �����ļ�
  bool upload_file();
  vector<string> get_files(string username, string pid); // ��ȡĿ¼�������ļ�
  bool create_file_dir(string username, string md5, string pid, string filename); // �����ļ�Ŀ¼
  bool delete_file(string fid, string did); // ɾ���ļ�
  bool move_file(string fid, string src, string dst); // �ƶ��ļ�
  int get_file_size(string fid); // ��ȡ�ļ���С
  string get_file_path(string fid); // ��ȡ�ļ�·��
  bool is_file_dir_exist(string pid, string filename); // �ж��ļ�Ŀ¼�Ƿ����
  bool copy_file(string username, string fid, string pid, string filename); // �ļ�����
  bool rename_file(string fid, string did, string newname); // �������ļ�
};