/* 
 * Copyright (C) 2006 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

log_define("pooldemo");

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    log_debug("logging initialized");

    const std::string database = argv[1];
    log_debug("database=" << database);

    tntdb::Connection conn1 = tntdb::connectCached(database);
    std::cout << "conn1=" << conn1.getImpl() << std::endl;

    tntdb::Connection conn2 = tntdb::connectCached(database);
    std::cout << "conn2=" << conn2.getImpl() << std::endl;

    conn1 = tntdb::connectCached(database);
    std::cout << "conn1=" << conn1.getImpl() << std::endl;

    conn2 = tntdb::connectCached(database);
    std::cout << "conn2=" << conn2.getImpl() << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
}

