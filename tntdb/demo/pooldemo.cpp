/* 
 * Copyright (C) 2006 Tommi Maekitalo
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

