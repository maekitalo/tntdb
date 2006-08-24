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

#include <tntdb/impl/poolconnection.h>
#include <tntdb/bits/connection.h>
#include <tntdb/bits/result.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/value.h>
#include <cxxtools/log.h>

log_define("tntdb.poolconnection");

namespace tntdb
{
  PoolConnection::PoolConnection(ConnectionPool::PoolObjectType connection_)
    : connection(connection_),
      inTransaction(false)
  {
    log_debug("PoolConnection " << this << " for connection " << connection->getImpl());
  }

  PoolConnection::~PoolConnection()
  {
    // don't put the connection back to the free pool, when there is a
    // pending transaction
    if (inTransaction)
    {
      log_debug("don't reuse connection" << connection->getImpl());
      connection.release();
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
}
