/*
 * Copyright (C) 2006 Tommi Maekitalo
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
#include <tntdb/connect.h>

log_define("pooldemo")

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    log_debug("logging initialized");

    if (argc <= 1)
    {
      std::cerr << "usage: " << argv[0] << " connectionstring" << std::endl;
      return 1;
    }

    const std::string database = argv[1];
    log_debug("database=" << database);

    log_debug("create connection 1");
    tntdb::Connection conn1 = tntdb::connectCached(database);
    std::cout << "conn1=" << conn1.getImpl() << std::endl;

    log_debug("create connection 2");
    tntdb::Connection conn2 = tntdb::connectCached(database);
    std::cout << "conn2=" << conn2.getImpl() << std::endl;

    log_debug("drop connection 1");
    conn1 = tntdb::Connection();

    log_debug("create connection 1");
    conn1 = tntdb::connectCached(database);
    std::cout << "conn1=" << conn1.getImpl() << std::endl;

    log_debug("drop connection 2 and replace with new");
    conn2 = tntdb::connectCached(database);
    std::cout << "conn2=" << conn2.getImpl() << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
}

