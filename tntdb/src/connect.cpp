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
