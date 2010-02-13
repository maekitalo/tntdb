/*
 * Copyright (C) 2005 Tommi Maekitalo
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
#include <algorithm>

#include <cxxtools/loginit.h>
#include <cxxtools/arg.h>

#include <tntdb/connect.h>
#include <tntdb/transaction.h>
#include <tntdb/value.h>
#include <tntdb/row.h>
#include <tntdb/result.h>
#include <tntdb/statement.h>

#include "util.h"

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> value(argc, argv, 'v');
    cxxtools::Arg<bool> row(argc, argv, 'r');
    cxxtools::Arg<bool> exec(argc, argv, 'e');
    cxxtools::Arg<bool> statement(argc, argv, 'p');
    cxxtools::Arg<bool> cursor(argc, argv, 'c');

    log_init();

    if (argc < 2)
    {
      std::cerr << "This is sqlcmd - a commandline-sql-processor. It demonstrates the use of tntdb.\n\n"
                   "Usage:\n"
                   "  sqlcmd [options] db-url [sql-query]\n\n"
                   "Options:\n"
                   "  -e        execute statement without result\n"
                   "  -v        select value\n"
                   "  -r        select row\n"
                   "  -p        select with prepared statement\n"
                   "  -c        select with cursor\n"
                   "When no sql-query is passed, sqlcmd reads a sql-query from standard-input.\n"
                << std::endl;
      return -1;
    }

    const std::string database = argv[1];
    std::string cmd;
    if (argc > 2)
      cmd = argv[2];
    else
    {
      // read sql-command from stdin
      std::ostringstream s;
      s << std::cin.rdbuf();   // copy stdin to buffer
      cmd = s.str();
    }

    tntdb::Connection conn = tntdb::connect(database);

    if (exec)
    {
      std::cout << "exec" << std::endl;
      conn.execute(cmd);
    }
    else if (value)
    {
      tntdb::Value v = conn.selectValue(cmd);
      ValuePrinter()(v);
      std::cout << std::endl;
    }
    else if (row)
    {
      tntdb::Row t = conn.selectRow(cmd);
      RowPrinter()(t);
    }
    else if (statement)
    {
      tntdb::Statement stmt = conn.prepare(cmd);
      tntdb::Result r = stmt.select();
      std::for_each(r.begin(), r.end(), RowPrinter());
    }
    else if (cursor)
    {
      // postgresql needs a active transaction for cursors
      tntdb::Transaction trans(conn);

      tntdb::Statement stmt = conn.prepare(cmd);
      std::for_each(stmt.begin(), stmt.end(), RowPrinter());
    }
    else
    {
      tntdb::Result r = conn.select(cmd);
      std::for_each(r.begin(), r.end(), RowPrinter());
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
}

