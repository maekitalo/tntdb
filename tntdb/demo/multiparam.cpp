#include <iostream>
#include <tntdb/connect.h>
#include <tntdb/statement.h>
#include <tntdb/row.h>
#include <cxxtools/log.h>
#include <vector>

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    tntdb::Connection conn = tntdb::connectCached("sqlite:mydb.db");

    std::vector<int> v;
    v.push_back(5);
    v.push_back(2);

    tntdb::Statement stmt = conn.prepare("select a from sometable where a in (" + tntdb::Statement::paramlist("v", v.size()) + ")");
    stmt.set("v", v);
    for (tntdb::Statement::const_iterator cur = stmt.begin(); cur != stmt.end(); ++cur)
    {
      int vv;
      (*cur).reader().get(vv);
      std::cout << vv << '\t';
    }
    std::cout << std::endl;

  }
  catch (const std::exception& e)
  {
    std::cerr << "failed: " << e.what() << std::endl;
  }
}

