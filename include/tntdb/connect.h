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

#ifndef TNTDB_CONNECT_H
#define TNTDB_CONNECT_H

#include <string>
#include <tntdb/connection.h>

namespace tntdb
{
  /**
   * Establish a connection to a database.
   *
   * The url is prefixed with a drivername followed by a colon and a driver-
   * specific part. If the connection can't be established, a exception of type
   * tntdb::Error or derived is thrown.
   *
   * Examples:
   *
   * \code
   *  tntdb::Connection sqConn = tntdb::connect("sqlite:mydatabase.db");
   *  tntdb::Connection pgConn = tntdb::connect("postgresql:dbname=DS2 user=web passwd=web");
   *  tntdb::Connection myConn = tntdb::connect("mysql:db=DS2;user=web;passwd=web");
   *  tntdb::Connection orConn = tntdb::connect("oracle:XE;user=hr;passwd=hr");
   * \endcode
   */
  Connection connect(const std::string& url);

  /**
   * Fetch a connection from a pool or create a new one.
   *
   * A static pool of connections is kept in memory. The function looks
   * in this pool, if there is a connection, which matches the url. If found
   * the connection is removed from the pool and returned. When the returned
   * connection-object is destroyed (and all copies of it), the actual
   * connection is put back into the pool.
   *
   * When there is no connections in the pool, which match the url, a new
   * connection is established.
   */
  Connection connectCached(const std::string& url);

  /**
   * Releases unused connections. Keeps the given number of
   * connections.
   */
  void dropCached(unsigned keep = 0);

  /**
   * Releases unused connections, which match the given url. Keeps
   * the given number of connections.
   */
  void dropCached(const std::string& url, unsigned keep = 0);

  /**
   * Sets the maximum pool-size of new connectionpools.
   *
   * When the maximum number of connections to a specific url is reached,
   * connectCached blocks until a connection is available.
   * The setting do not affect pools with active connections. You should
   * release all connections and drop clear the pool to your url with
   * dropCached(url), which recreates the pool.
   */
  void setMaxPoolSize(unsigned max);

  /**
   * Returns the current setting for maximum pools size.
   *
   * The maximum pools size is used, when a new pool is created
   */
  unsigned getMaxPoolSize();
}

#endif // TNTDB_CONNECT_H

