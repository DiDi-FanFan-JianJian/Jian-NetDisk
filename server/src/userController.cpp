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
  string sql = "insert into users (username, password) values ('" + username + "', '" + password + "')";
  if (!insert(sql))
  {
    return false;
  }
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