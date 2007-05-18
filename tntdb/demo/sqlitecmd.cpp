#include <exception>
#include <iostream>
#include <cxxtools/loginit.h>
#include <cxxtools/log.h>

#include <tntdb/sqlite/connection.h>
#include <tntdb/sqlite/value.h>
#include <tntdb/sqlite/row.h>
#include <tntdb/sqlite/result.h>

#include <cxxtools/arg.h>
#include <algorithm>

#include "util.h"

log_define("sqlitecmd.main")

namespace db
{
  using namespace tntdb::sqlite;
}

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> value(argc, argv, 'v');
    cxxtools::Arg<bool> row(argc, argv, 'r');
    cxxtools::Arg<bool> exec(argc, argv, 'e');

    cxxtools::Arg<bool> debug(argc, argv, 'd');
    if (debug)
      log_init_debug();
    else
      log_init();

    if (argc != 3)
    {
      std::cerr << "usage: " << argv[0] << " database command" << std::endl;
      return -1;
    }

    const char* database = argv[1];
    const char* cmd = argv[2];

    db::Connection conn(database);

    if (value)
    {
      db::Value v = conn.selectValue(cmd);
      Valueprinter<db::Value>()(v);
      std::cout << std::endl;
    }
    else if (row)
    {
      db::Row t = conn.selectRow(cmd);
      RowPrinter<db::Row>()(t);
    }
    else if (exec)
      conn.exec(argv[2]);
    else
    {
      db::Result r = conn.select(cmd);
      std::for_each(r.begin(), r.end(), RowPrinter<db::Row>());
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
}

