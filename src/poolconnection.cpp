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

#include <tntdb/impl/poolconnection.h>
#include <tntdb/bits/connection.h>
#include <tntdb/bits/result.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/value.h>
#include <cxxtools/log.h>

log_define("tntdb.poolconnection")

namespace tntdb
{
PoolConnection::PoolConnection(std::shared_ptr<IConnection>&& connection, ConnectionPool& connectionPool)
    : _connectionPool(connectionPool),
      _connection(connection),
      _inTransaction(false),
      _drop(false)
{
    log_debug("PoolConnection " << this << " for connection " << _connection);
}

PoolConnection::~PoolConnection()
{
    // don't put the connection back to the free pool, when there is a
    // pending transaction or something unusual has happened
    if (_inTransaction || _drop)
        log_debug("don't reuse connection " << _connection);
   else
       _connectionPool.put(_connection);
}

void PoolConnection::beginTransaction()
{
    _connection->beginTransaction();
    _inTransaction = true;
}

void PoolConnection::commitTransaction()
{
    _connection->commitTransaction();
    _inTransaction = false;
}

void PoolConnection::rollbackTransaction()
{
    _connection->rollbackTransaction();
    _inTransaction = false;

    // When a rollback has been done, this may be a indication, that something
    // weird has happened, so we do not reuse that connection.

    _drop = true;
}

PoolConnection::size_type PoolConnection::execute(const std::string& query)
{
    return _connection->execute(query);
}

Result PoolConnection::select(const std::string& query)
{
    return _connection->select(query);
}

Row PoolConnection::selectRow(const std::string& query)
{
    return _connection->selectRow(query);
}

Value PoolConnection::selectValue(const std::string& query)
{
    return _connection->selectValue(query);
}

Statement PoolConnection::prepareWithLimit(const std::string& query, const std::string& limit, const std::string& offset)
{
    return _connection->prepareWithLimit(query, limit, offset);
}

Statement PoolConnection::prepare(const std::string& query)
{
    return _connection->prepare(query);
}

Statement PoolConnection::prepareCached(const std::string& query, const std::string& key)
{
    return _connection->prepareCached(query, key);
}

Statement PoolConnection::prepareCachedWithLimit(const std::string& query, const std::string& limit, const std::string& offset, const std::string& key)
{
    return _connection->prepareCachedWithLimit(query, limit, offset, key);
}

void PoolConnection::clearStatementCache()
{
    return _connection->clearStatementCache();
}

bool PoolConnection::clearStatementCache(const std::string& key)
{
    return _connection->clearStatementCache(key);
}

bool PoolConnection::ping()
{
    bool ok = _connection->ping();
    if (!ok)
        _drop = true;
    return ok;
}

long PoolConnection::lastInsertId(const std::string& name)
{
    return _connection->lastInsertId(name);
}

void PoolConnection::lockTable(const std::string& tablename, bool exclusive)
{
    _connection->lockTable(tablename, exclusive);
}

}
