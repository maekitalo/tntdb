#include <exception>
#include <iostream>
#include <cxxtools/loginit.h>
#include <cxxtools/log.h>

#include <tntdb/connect.h>
#include <tntdb/transaction.h>
#include <tntdb/value.h>
#include <tntdb/row.h>
#include <tntdb/result.h>
#include <tntdb/statement.h>

#include <cxxtools/arg.h>
#include <algorithm>
#include "util.h"

log_define("mysqlcmd.main")

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> value(argc, argv, 'v');
    cxxtools::Arg<bool> row(argc, argv, 'r');
    cxxtools::Arg<bool> exec(argc, argv, 'e');
    cxxtools::Arg<bool> prepare(argc, argv, 'p'); // only testing

    cxxtools::Arg<bool> debug(argc, argv, 'd');
    if (debug)
      log_init_debug();
    else
      log_init();

    if (argc < 2)
    {
      std::cerr << "usage: " << argv[0] << " database [command]" << std::endl;
      return -1;
    }

    const std::string database = argv[1];
    const char* cmd = argv[2];

    tntdb::Connection conn = tntdb::connect("mysql:" + database);

    if (exec)
    {
      std::cout << "exec" << std::endl;
      conn.execute(argv[2]);
    }
    else if (value)
    {
      std::cout << "selectValue" << std::endl;
      tntdb::Value v = conn.selectValue(cmd);
      Valueprinter<tntdb::Value>()(v);
      std::cout << std::endl;
    }
    else if (row)
    {
      std::cout << "selectRow" << std::endl;
      tntdb::Row t = conn.selectRow(cmd);
      RowPrinter<tntdb::Row>()(t);
    }
    else if (prepare)
    {
      tntdb::Statement stmt = conn.prepare(cmd);
      tntdb::Result r = stmt.select();
      std::for_each(r.begin(), r.end(), RowPrinter<tntdb::Row>());
    }
    else
    {
      std::cout << "select" << std::endl;
      tntdb::Result r = conn.select(cmd);
      std::for_each(r.begin(), r.end(), RowPrinter<tntdb::Row>());
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
}

