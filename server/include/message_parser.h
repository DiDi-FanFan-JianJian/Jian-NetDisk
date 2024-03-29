#pragma once

#include <string>
#include <iostream>
#include <map>

#include "message.h"
#include "dbController.h"
#include "md5.h"

using namespace std;

class router {
private:
  dbController* db;
  map<string, int> tot_block;
  map<string, int> cur_block;

public:
  router(string user, string passwd, string dbname);
  ~router() {};

  string parse(const char* msg);

  // user
  string handle_login(const char* msg);
  string handle_reg(const char* msg);

  // file
  string handle_upload_file(const char* msg);
  string handle_upload_block(const char* msg);
  string handle_create_file_dir(const char* msg);
  string handle_get_file_id(const char* msg);
  string handle_get_files(const char* msg);
  string handle_move_file(const char* msg);
  string handle_delete_file(const char* msg);
  string handle_get_file_info(const char* msg);
  string handle_download_block(const char* msg);
  string handle_copy_file(const char* msg);
  string handle_rename_file(const char* msg);

  // dir
  string handle_create_dir(const char* msg);
  string handle_get_dir_id(const char* msg);
  string handle_get_dirs(const char* msg);
  string handle_move_dir(const char* msg);
  string handle_delete_dir(const char* msg);
  string handle_copy_dir(const char* msg);
  string handle_rename_dir(const char* msg);


  string handel_get_file_all_info(const char* msg);
};