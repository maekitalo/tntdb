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

#include <tntdb/connect.h>
#include <tntdb/connection.h>
#include <tntdb/error.h>
#include <tntdb/librarymanager.h>
#include <tntdb/iface/iconnectionmanager.h>
#include <tntdb/connectionpool.h>
#include <cxxtools/log.h>
#include <map>
#include "config.h"

#ifdef WITH_STATIC_POSTGRESQL
# include <tntdb/postgresql/impl/connectionmanager.h>
#endif

#ifdef WITH_STATIC_MYSQL
# include <tntdb/mysql/impl/connectionmanager.h>
#endif

#ifdef WITH_STATIC_SQLITE
# include <tntdb/sqlite/impl/connectionmanager.h>
#endif

log_define("tntdb.connect");

namespace tntdb
{
  typedef std::map<std::string, LibraryManager> librariesType;
  static librariesType libraries;

  Connection connect(const std::string& url)
  {
    log_debug("connect(\"" << url << "\")");

    std::string::size_type n = url.find(':');
    if (n == std::string::npos)
#ifdef WITH_STATIC_POSTGRESQL
      return connectionManager_postgresql.connect(url);
#elif WITH_STATIC_MYSQL
      return connectionManager_mysql.connect(url);
#elif WITH_STATIC_SQLITE
      return connectionManager_sqlite.connect(url);
#else
      throw Error("invalid url \"" + url + '"');
#endif

    std::string driverName =  url.substr(0, n);
#ifdef WITH_STATIC_POSTGRESQL
    if (driverName == "postgresql")
      return connectionManager_postgresql.connect(url.substr(n + 1));
#elif WITH_STATIC_MYSQL
    if (driverName == "mysql")
      return connectionManager_mysql.connect(url.substr(n + 1));
#elif WITH_STATIC_SQLITE
    if (driverName == "sqlite")
      return connectionManager_sqlite.connect(url.substr(n + 1));
#endif

    std::string libraryUrl = url.substr(n + 1);
    log_debug("driver \"" << driverName << "\" url=\"" << libraryUrl << '"');

    // lookup library-manager
    LibraryManager libraryManager;
    librariesType::const_iterator it = libraries.find(driverName);
    if (it == libraries.end())
    {
      libraryManager = LibraryManager(driverName);
      libraries[driverName] = libraryManager;
    }
    else
      libraryManager = it->second;

    return libraryManager.connect(libraryUrl);
  }

  static ConnectionPool connectionPool;

  Connection connectCached(const std::string& url)
  {
    log_debug("connectCached(\"" << url << "\")");
    return connectionPool.connect(url);
  }

  void dropCached(unsigned keep)
  {
    connectionPool.drop(keep);
  }

  void dropCached(const std::string& url, unsigned keep)
  {
    connectionPool.drop(url, keep);
  }

}
