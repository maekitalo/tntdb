/*
 * Copyright (C) 2006 Tommi Maekitalo
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

#ifndef TNTDB_CONNECTIONPOOL_H
#define TNTDB_CONNECTIONPOOL_H

#include <cxxtools/noncopyable.h>
#include <cxxtools/pool.h>
#include <map>
#include <string>

namespace tntdb
{
  class Connection;

  class ConnectionPool
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

      typedef cxxtools::Pool<Connection, Connector> PoolType;
      PoolType pool;

    public:
      typedef PoolType::Ptr PoolObjectType;

    public:
      explicit ConnectionPool(const std::string& url, unsigned maxcount = 0)
        : pool(maxcount, Connector(url))
        { }

      Connection connect();

      /**
       * Releases unused connections. Keeps the given number of
       * connections.
       */
      void drop(unsigned keep = 0)     { pool.drop(keep); }

      unsigned getMaximumSize()        { return pool.getMaximumSize(); }
      void setMaximumSize(unsigned m)  { pool.setMaximumSize(m); }
      unsigned getCurrentSize() const  { return pool.getCurrentSize(); }
  };

  class ConnectionPools : private cxxtools::NonCopyable
  {
    public:
      typedef ConnectionPool PoolType;
      typedef std::map<std::string, PoolType*> PoolsType;

    private:
      PoolsType pools;
      unsigned maxcount;
      mutable cxxtools::Mutex mutex;

    public:
      explicit ConnectionPools(unsigned maxcount_ = 0)
        : maxcount(maxcount_)
        { }
      ~ConnectionPools();

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

      unsigned getMaximumSize()            { return maxcount; }
      void setMaximumSize(unsigned m);
      unsigned getCurrentSize(const std::string& url) const;
  };

}

#endif // TNTDB_CONNECTIONPOOL_H

