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

#ifndef TNTDB_CONNECTIONPOOL_H
#define TNTDB_CONNECTIONPOOL_H

#include <tntdb/noncopyable.h>
#include <cxxtools/pool.h>
#include <map>

namespace tntdb
{
  class Connection;

  class ConnectionPool : private NonCopyable
  {
      class Connector
      {
          std::string url;
        public:
          Connector(const std::string& url_)
            : url(url_)
            { }

          Connection* operator() ();
      };

    public:
      typedef cxxtools::Pool<Connection, Connector> PoolType;
      typedef std::map<std::string, PoolType*> PoolsType;
      typedef PoolType::PoolObject PoolObjectType;

    private:
      PoolsType pools;
      unsigned maxcount;
      mutable cxxtools::Mutex mutex;

    public:
      explicit ConnectionPool(unsigned maxcount_ = 0)
        : maxcount(maxcount_)
        { }
      ~ConnectionPool();

      Connection connect(const std::string& url);

      /**
       * Releases unused connections. Keeps the given number of
       * connections.
       */
      void drop(unsigned keep = 0);
      /**
       * Releases unused connections, which match the given url. Keeps
       * the given number of connections.
       */
      void drop(const std::string& url, unsigned keep = 0);

      unsigned getMaxSize()            { return maxcount; }
      void setMaxSize(unsigned m);
      unsigned getCurrentSize(const std::string& url) const;
  };

}

#endif // TNTDB_CONNECTIONPOOL_H

