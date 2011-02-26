/*
 * 
 * 
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

