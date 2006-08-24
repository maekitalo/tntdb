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

