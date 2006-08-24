/* 
 * Copyright (C) 2006 Tommi Maekitalo
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

#include <tntdb/connectionpool.h>
#include <tntdb/connect.h>
#include <tntdb/impl/poolconnection.h>
#include <cxxtools/log.h>

log_define("tntdb.connectionpool");

namespace tntdb
{
  Connection* ConnectionPool::Connector::operator() ()
  {
    return new Connection(tntdb::connect(url));
  }

  ConnectionPool::~ConnectionPool()
  {
    cxxtools::MutexLock lock(mutex);
    for (PoolsType::iterator it = pools.begin(); it != pools.end(); ++it)
      delete it->second;
  }

  Connection ConnectionPool::connect(const std::string& url)
  {
    log_debug("ConnectionPool::connect(\"" << url << "\")");

    PoolsType::iterator it;

    {
      cxxtools::MutexLock lock(mutex);
      it = pools.find(url);
      if (it == pools.end())
      {
        log_debug("create pool for url \"" << url << "\"");
        PoolType* pool = new PoolType(maxcount, Connector(url));
        it = pools.insert(PoolsType::value_type(url, pool)).first;
      }
      else
        //log_debug("pool for url \"" << url << "\" found; current size=" << it->second->getCurrentSize());
        log_debug("pool for url \"" << url << "\" found");
    }

    log_debug("current pool-size " << it->second->getCurrentSize());

    return Connection(new PoolConnection(it->second->get()));
  }

  void ConnectionPool::drop(unsigned keep)
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

  void ConnectionPool::drop(const std::string& url, unsigned keep)
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

  unsigned ConnectionPool::getCurrentSize(const std::string& url) const
  {
    cxxtools::MutexLock lock(mutex);

    PoolsType::const_iterator it = pools.find(url);
    return it == pools.end() ? 0
                             : it->second->getCurrentSize();
  }
}
