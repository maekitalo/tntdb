#include <exception>
#include <iostream>

#include <tntdb/connect.h>
#include <tntdb/value.h>
#include <tntdb/statement.h>
#include <tntdb/row.h>
#include <tntdb/result.h>
#include <cxxtools/arg.h>
#include <cxxtools/loginit.h>
#include <cxxtools/log.h>

#include "util.h"

log_define("categories")

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> debug(argc, argv, 'd');
    if (debug)
      log_init_debug();
    else
      log_init();

    cxxtools::Arg<bool> use_cursor(argc, argv, 'c');

    if (argc != 3)
    {
      std::cerr << "usage: " << argv[0] << " databasefile name" << std::endl;
      return -1;
    }

    std::string db = argv[1];
    std::string name = argv[2];

    log_debug("connect");
    tntdb::Connection conn = tntdb::connect("mysql:" + db);

    log_debug("prepare");
    tntdb::Statement st = conn.prepare(
      "select CATEGORY, CATEGORYNAME"
      "  from CATEGORIES"
      " where CATEGORYNAME like :name");

    log_debug("set parameter");
    st.setString("name", name + '%');

    if (use_cursor)
    {
      log_debug("iterate with cursor");
      for (tntdb::Statement::const_iterator it = st.begin(); it != st.end(); ++it)
      {
        log_debug("fetch data");
        int category = it->getInt(0);
        std::string categoryname = it->getString(1);
        std::cout << category << '\t' << categoryname << std::endl;
      }
    }
    else
    {
      log_debug("select");
      tntdb::Result res = st.select();

      log_debug("print");
      for (tntdb::Result::const_iterator it = res.begin(); it != res.end(); ++it)
      {
        int category = it->getInt(0);
        std::string categoryname = it->getString(1);
        std::cout << category << '\t' << categoryname << std::endl;
      }
    }

    //std::for_each(res.begin(), res.end(), RowPrinter<tntdb::Row>());
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

