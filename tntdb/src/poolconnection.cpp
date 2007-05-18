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

#include <tntdb/impl/poolconnection.h>
#include <tntdb/bits/connection.h>
#include <tntdb/bits/result.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/value.h>
#include <cxxtools/log.h>

log_define("tntdb.poolconnection")

namespace tntdb
{
  PoolConnection::PoolConnection(ConnectionPool::PoolObjectType connection_)
    : connection(connection_),
      inTransaction(false),
      drop(false)
  {
    log_debug("PoolConnection " << this << " for connection " << connection->getImpl());
  }

  PoolConnection::~PoolConnection()
  {
    // don't put the connection back to the free pool, when there is a
    // pending transaction
    if (inTransaction || drop)
    {
      log_debug("don't reuse connection " << connection->getImpl());
      connection.release(false);
    }
    else
      log_debug("reuse connection " << connection->getImpl());
  }

  void PoolConnection::beginTransaction()
  {
    connection->beginTransaction();
    inTransaction = true;
  }

  void PoolConnection::commitTransaction()
  {
    connection->commitTransaction();
    inTransaction = false;
  }

  void PoolConnection::rollbackTransaction()
  {
    connection->rollbackTransaction();
    // keep transaction state
  }

  PoolConnection::size_type PoolConnection::execute(const std::string& query)
  {
    return connection->execute(query);
  }

  Result PoolConnection::select(const std::string& query)
  {
    return connection->select(query);
  }

  Row PoolConnection::selectRow(const std::string& query)
  {
    return connection->selectRow(query);
  }

  Value PoolConnection::selectValue(const std::string& query)
  {
    return connection->selectValue(query);
  }

  Statement PoolConnection::prepare(const std::string& query)
  {
    return connection->prepare(query);
  }

  Statement PoolConnection::prepareCached(const std::string& query)
  {
    return connection->prepareCached(query);
  }

  void PoolConnection::clearStatementCache()
  {
    return connection->clearStatementCache();
  }

  bool PoolConnection::ping()
  {
    bool ok = connection->ping();
    if (!ok)
      drop = true;
    return ok;
  }
}
