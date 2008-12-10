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

#include <tntdb/connectionpool.h>
#include <tntdb/connect.h>
#include <tntdb/impl/poolconnection.h>
#include <cxxtools/log.h>

log_define("tntdb.connectionpool")

namespace tntdb
{
  ////////////////////////////////////////////////////////////////////////
  // Connector
  //
  Connection* ConnectionPool::Connector::operator() ()
  {
    log_debug("create new connection for url \"" << url << '"');
    return new Connection(tntdb::connect(url));
  }

  ////////////////////////////////////////////////////////////////////////
  // ConnectionPool
  //
  Connection ConnectionPool::connect()
  {
    log_debug("ConnectionPool::connect()");

    log_debug("current pool-size " << getCurrentSize());

    while (true)
    {
      Connection conn(new PoolConnection(pool.get()));

      if (conn.ping())
        return conn;

      // a pool-connection don't put itself back into the pool after a failed ping
      log_warn("drop dead connection from pool");
    }

    return Connection();  // never reached
  }

  ////////////////////////////////////////////////////////////////////////
  // ConnectionPools
  //
  ConnectionPools::~ConnectionPools()
  {
    cxxtools::MutexLock lock(mutex);
    for (PoolsType::iterator it = pools.begin(); it != pools.end(); ++it)
      delete it->second;
  }

  Connection ConnectionPools::connect(const std::string& url)
  {
    log_debug("ConnectionPools::connect(\"" << url << "\")");

    PoolsType::iterator it;

    {
      cxxtools::MutexLock lock(mutex);
      it = pools.find(url);
      if (it == pools.end())
      {
        log_debug("create pool for url \"" << url << "\" with " << maxcount << " connections");
        PoolType* pool = new PoolType(url, maxcount);
        it = pools.insert(PoolsType::value_type(url, pool)).first;
      }
      else
        log_debug("pool for url \"" << url << "\" found");
    }

    log_debug("current pool-size " << it->second->getCurrentSize());
    return it->second->connect();
  }

  void ConnectionPools::drop(unsigned keep)
  {
    log_debug("drop(" << keep << ')');

    cxxtools::MutexLock lock(mutex);
    for (PoolsType::iterator it = pools.begin(); it != pools.end(); ++it)
    {
      log_debug("pool \"" << it->first << "\"; current size " << it->second->getCurrentSize());
      it->second->drop();
      log_debug("connections released " << it->second->getCurrentSize() << " kept");
    }
  }

  void ConnectionPools::drop(const std::string& url, unsigned keep)
  {
    log_debug("drop(\"" << url << "\", " << keep << ')');

    cxxtools::MutexLock lock(mutex);

    PoolsType::iterator it = pools.find(url);
    if (it != pools.end())
    {
      log_debug("pool \"" << url << "\" found; current size " << it->second->getCurrentSize());
      it->second->drop(keep);
      log_debug("connections released " << it->second->getCurrentSize() << " kept");

      if (it->second->getCurrentSize() == 0)
      {
        log_debug("delete connectionpool for \"" << url << "\"");
        delete it->second;
        pools.erase(it);
      }
    }
    else
      log_debug("pool \"" << url << "\" not found");
  }

  unsigned ConnectionPools::getCurrentSize(const std::string& url) const
  {
    cxxtools::MutexLock lock(mutex);

    PoolsType::const_iterator it = pools.find(url);
    return it == pools.end() ? 0
                             : it->second->getCurrentSize();
  }

  void ConnectionPools::setMaximumSize(unsigned m)
  {
    cxxtools::MutexLock lock(mutex);
    maxcount = m;
    for (PoolsType::const_iterator it = pools.begin(); it != pools.end(); ++it)
      it->second->setMaximumSize(m);
  }
}
