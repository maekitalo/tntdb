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

#include <tntdb/connect.h>
#include <tntdb/connection.h>
#include <tntdb/statement.h>
#include <tntdb/result.h>
#include <tntdb/transaction.h>
#include <cxxtools/arg.h>
#include <cxxtools/loginit.h>

#include "util.h"

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> cursor(argc, argv, 'c');
    cxxtools::Arg<bool> debug(argc, argv, 'd');

    if (debug)
      log_init_debug();
    else
      log_init();

    if (argc != 3)
    {
      std::cerr << "usage: " << argv[0] << " db-url name" << std::endl;
      return -1;
    }

    std::string database = argv[1];
    std::string name = argv[2];

    tntdb::Connection conn = tntdb::connect(database);

    tntdb::Statement st = conn.prepare(
      "select per_pid, per_nachname, per_vorname, per_verein"
      "  from person"
      " where per_nachname like :Name"
      "    or per_vorname like :Name");
    st.set("Name", name + '%');

    if (cursor)
      std::for_each(st.begin(), st.end(), RowPrinter());
    else
    {
      tntdb::Result res = st.select();
      std::for_each(res.begin(), res.end(), RowPrinter());
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

