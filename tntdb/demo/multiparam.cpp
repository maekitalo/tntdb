#include <iostream>
#include <tntdb/connect.h>
#include <tntdb/statement.h>
#include <tntdb/sqlbuilder.h>
#include <tntdb/row.h>
#include <cxxtools/log.h>
#include <vector>
#include <set>

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    tntdb::Connection conn = tntdb::connectCached("sqlite:mydb.db");

    std::vector<int> v;
    v.push_back(5);
    v.push_back(2);

    std::set<double> w;

    tntdb::Statement stmt = conn.prepare(
                                tntdb::SqlBuilder("select a from %table where a in (%v) or b in (%w) and z = :z %q1 %q2")
                                    .extendParam("v", v.size())
                                    .extendParam("w", w.size())
                                    .replace("table", mytable)
                                    .replaceIf(condition1, "q1", "and z > :z")
                                    .replaceIf(condition2, "q2", "and y > :y")
                                    );

    stmt.set("v", v);
    stmt.set("w", w);
    if (condition1)
      stmt.set("z", 5);
    if (condition2)
      stmt.set("y", 18);

    for (tntdb::Statement::const_iterator cur = stmt.begin(); cur != stmt.end(); ++cur)
    {
      int vv;
      (*cur)[0].get(vv);
      std::cout << vv << '\t';
    }
    std::cout << std::endl;

  }
  catch (const std::exception& e)
  {
    std::cerr << "failed: " << e.what() << std::endl;
  }
}

