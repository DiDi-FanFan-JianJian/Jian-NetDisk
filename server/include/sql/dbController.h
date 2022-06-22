#include <string>
#include <mysql/mysql.h>
using namespace std;

class Controller {
private:
  MYSQL* conn;
public:
  Controller(string user, string passwd);
  ~Controller();
};