/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
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

