/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <string>

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

class sql_cls
{
private:
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

public:
    void dummy_function();
    sql_cls(string host, string uname, string pwd);
    bool update_table(string query);
    ~sql_cls();
};