/* 
   Copyright (C) 2006 Tommi Maekitalo

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

      unsigned getCurrentSize(const std::string& url) const;
  };

}

#endif // TNTDB_CONNECTIONPOOL_H

