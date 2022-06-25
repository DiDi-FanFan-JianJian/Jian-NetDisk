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
};