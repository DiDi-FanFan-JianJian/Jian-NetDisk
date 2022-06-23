#include "dbController.h"

dbController::dbController(string user, string passwd, string db)
{
  if ((conn = mysql_init(NULL)) == NULL)
  {
    printf("mysql_init failed\n");
    exit(1);
  }

  // connect to database
  if (!mysql_real_connect(conn, "localhost", user.c_str(), passwd.c_str(), db.c_str(), 0, NULL, 0))
  {
    cout << "mysql_real_connect failed(" << mysql_errno(conn) << "):" << mysql_error(conn) << endl;
    exit(1);
  }

  cout << "connect to database success" << endl;

  // ÉèÖÃ×Ö·û¼¯
  mysql_set_character_set(conn, "gbk");
}

bool dbController::query(string sql)
{
  if (mysql_query(conn, sql.c_str()))
  {
    cout << "mysql_query failed(" << mysql_errno(conn) << "):" << mysql_error(conn) << endl;
    return false;
  }

  res = mysql_store_result(conn);
  int num_fields = mysql_num_fields(res);
  result.clear();
  while ((row = mysql_fetch_row(res)))
  {
    vector<string> tmp;
    for (int i = 0; i < num_fields; i++)
    {
      tmp.push_back(row[i]);
    }
    result.push_back(tmp);
  }
  
  return true;
}

bool dbController::insert(string sql)
{
  if (mysql_query(conn, sql.c_str()))
  {
    cout << "mysql_query failed(" << mysql_errno(conn) << "):" << mysql_error(conn) << endl;
    return false;
  }
  return true;
}

void dbController::printResult()
{
  int num_fields = mysql_num_fields(res);
  int num_rows = mysql_num_rows(res);
  cout << "num_fields: " << num_fields << endl;
  cout << "num_rows: " << num_rows << endl;
  for (int i = 0; i < num_rows; i++)
  {
    for (int j = 0; j < num_fields; j++)
    {
      cout << result[i][j] << " ";
    }
    cout << endl;
  }
}

dbController::~dbController()
{
  mysql_close(conn);
}