/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#include <exception>
#include <iostream>
#include <algorithm>

#include <cxxtools/loginit.h>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>

#include <tntdb/connect.h>
#include <tntdb/transaction.h>
#include <tntdb/value.h>
#include <tntdb/row.h>
#include <tntdb/result.h>
#include <tntdb/statement.h>

#include "util.h"

log_define("sqlcmd.main");

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> value(argc, argv, 'v');
    cxxtools::Arg<bool> row(argc, argv, 'r');
    cxxtools::Arg<bool> exec(argc, argv, 'e');
    cxxtools::Arg<bool> statement(argc, argv, 'p');
    cxxtools::Arg<bool> cursor(argc, argv, 'c');

    cxxtools::Arg<bool> debug(argc, argv, 'd');
    if (debug)
      log_init_debug();
    else
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
                   "  -d        enable debug\n\n"
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

