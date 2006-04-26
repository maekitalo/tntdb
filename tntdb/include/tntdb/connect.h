/* 
   Copyright (C) 2005 Tommi Maekitalo

This file is part of tntdb.

Tntdb is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntdb is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntdb; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
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
   *  tntdb::Connection myConn = tntdb::connect("mysql:db=DS2;user=web;passwd=web");
   *  tntdb::Connection pgConn = tntdb::connect("postgresql:dbname=DS2 user=web passwd=web");
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
}

#endif // TNTDB_CONNECT_H

