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

