/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#include <iostream>
#include <tntdb/connect.h>
#include <tntdb/connection.h>
#include <tntdb/transaction.h>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>
#include <cxxtools/thread.h>

log_define("locktest")

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<unsigned> wait(argc, argv, 'w', 0);
    cxxtools::Arg<bool> readLock(argc, argv, 'r');

    if (argc < 3)
    {
      std::cerr << "usage: " << argv[0] << " dburl {tables}\n" << std::endl;
      return 1;
    }

    const char* dburl = argv[1];

    log_init();
    std::cout << "connect" << std::endl;
    tntdb::Connection conn = tntdb::connect(dburl);

    std::cout << "start transaction" << std::endl;
    tntdb::Transaction trans(conn);

    for (int a = 2; a < argc; ++a)
    {
      const char* tablename = argv[a];
      log_info("lock table " << tablename);
      trans.lockTable(tablename, !readLock);
      log_info("table " << tablename << " locked");
      cxxtools::Thread::sleep(wait);
    }

    trans.commit();
    log_info("transaction finished");
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

