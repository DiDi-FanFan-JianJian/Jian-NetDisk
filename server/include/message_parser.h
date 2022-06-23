#pragma once

#include <string>
#include <iostream>

#include "message.h"
#include "dbController.h"

using namespace std;

class router {
private:
  dbController* db;

public:
  router(string user, string passwd, string dbname);
  ~router() {};

  string parse(const char* msg);

  // user
  string handle_login(const char* msg);
  string handle_reg(const char* msg);
};