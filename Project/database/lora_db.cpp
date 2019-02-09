// g++ -std=c++11 -c -L .../lib64 sql_update.cpp -lmysqlcppconn -o sql_proc.o

#include "lora_db.hpp"

/* host:  ip_address of the server
 * uname: username of the database
 * pwd:   password of the database */ 
sql_cls::sql_cls(string host, string uname, string pwd)
{
  driver = get_driver_instance(); 
  con = driver->connect(host, uname, pwd); // Create a connection
  // TODO: the database to connect should be mentioned below.
  con->setSchema("lora_test"); // Connect to the MySQL test database
  stmt = con->createStatement();
}

/* query: should be an insert query that will update the table */
bool sql_cls::update_table(string query)
{
    // mostly it should be an insert query
    try 
    {
        stmt->execute(query);
    }
    catch (sql::SQLException &e) 
    {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "  << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        return false;
    }
    return true;
}

sql_cls::~sql_cls()
{
//   delete res;
  delete stmt;
  delete con;
}

void sql_cls::dummy_function(void)
{
  res = stmt->executeQuery("SELECT * from m122018");
  while (res->next()) {
    cout << "\t... MySQL replies: ";
    /* Access column data by alias or column name */
    cout << res->getString("lid") << res->getString("sid") << endl;
    cout << "\t... MySQL says it again: ";
    /* Access column data by numeric offset, 1 is the first column */
    cout << res->getString(1) << endl;
  }
//   stmt->execute("INSERT INTO books(title, price) VALUES ('book',43)");
}

// Uncomment below function to test
// int main()
// {
//     cout << "hello world" << endl;
//     sql_cls mysql("127.0.0.1", "root", "samaritan3");
//     mysql.dummy_function();
// }