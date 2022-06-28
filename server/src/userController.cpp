#include "dbController.h"


bool dbController::login(string username, string password)
{
  string sql = "select * from users where username = '" + username + "' and password = '" + password + "'";
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

bool dbController::reg(string username, string password)
{
  string did = getNextId("dirs");
  string sql = "insert into users (username, password, root) values ('" + username + "', '" + password + "', '" + did + "')";
  if (!insert(sql))
  {
    return false;
  }
  create_dir(username, "0", "root");
  return true;
}

bool dbController::is_user_exist(string username)
{
  string sql = "select * from users where username = '" + username + "'";
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

string dbController::get_user_id(string username)
{
  string sql = "select id from users where username = '" + username + "'";
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