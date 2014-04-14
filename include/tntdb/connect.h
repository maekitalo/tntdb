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
  /** Establish a connection to a database

      The url is prefixed with a driver name followed by a colon and a driver-specific part. 
      If the connection can't be established, an exception derived from tntdb::Error is thrown.

      Examples:

      @code
        tntdb::Connection sqConn = tntdb::connect("sqlite:mydatabase.db");
        tntdb::Connection pgConn = tntdb::connect("postgresql:dbname=DS2 user=web passwd=web");
        tntdb::Connection myConn = tntdb::connect("mysql:db=DS2;user=web;passwd=web");
        tntdb::Connection orConn = tntdb::connect("oracle:XE;user=hr;passwd=hr");
      @endcode
   */
  Connection connect(const std::string& url);

  /** Fetch a connection from a pool or create a new one

      A static pool of connections is kept in memory. This function looks
      in this pool if there is a connection with the given url. If found,
      the connection is removed from the pool and returned. When the returned
      Connection object is destroyed (and all copies are too), the actual
      connection is put back into the pool.

      When there is no connection with the given url in the pool, a new
      connection is established.
   */
  Connection connectCached(const std::string& url);

  /// Release unused connections; keep the given number of connections
  void dropCached(unsigned keep = 0);

  /** Release unused connections with the given database url; keep the
      given number of connections
   */
  void dropCached(const std::string& url, unsigned keep = 0);

  /** Set the maximum pool size for new connection pools

      When the maximum number of connections to a specific url is reached,
      connectCached blocks until a connection is available.

      This setting do not affect pools with active connections. You have to
      release all connections and clear the pool of all connections to your
      database with dropCached(url) for this setting to take effect.
   */
  void setMaxPoolSize(unsigned max);

  /// Get the current setting for maximum pool size (see setMaxPoolSize())
  unsigned getMaxPoolSize();
}

#endif // TNTDB_CONNECT_H

