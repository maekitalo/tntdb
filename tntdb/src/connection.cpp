/*
 * Copyright (C) 2005 Tommi Maekitalo
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

#include <tntdb/connection.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <tntdb/statement.h>
#include <cxxtools/log.h>

log_define("tntdb.connection")

namespace tntdb
{
  void Connection::beginTransaction()
  {
    log_trace("Connection::beginTransaction()");

    conn->beginTransaction();
  }

  void Connection::commitTransaction()
  {
    log_trace("Connection::commitTransaction");

    conn->commitTransaction();
  }

  void Connection::rollbackTransaction()
  {
    log_trace("Connection::rollbackTransaction");

    conn->rollbackTransaction();
  }

  Connection::size_type Connection::execute(const std::string& query)
  {
    log_trace("Connection::execute(\"" << query << "\")");

    return conn->execute(query);
  }

  Result Connection::select(const std::string& query)
  {
    log_trace("Connection::select(\"" << query << "\")");

    return conn->select(query);
  }

  Row Connection::selectRow(const std::string& query)
  {
    log_trace("Connection::selectRow(\"" << query << "\")");

    return conn->selectRow(query);
  }

  Value Connection::selectValue(const std::string& query)
  {
    log_trace("Connection::selectValue(\"" << query << "\")");

    return conn->selectValue(query);
  }

  Statement Connection::prepare(const std::string& query)
  {
    log_trace("Connection::prepare(\"" << query << "\")");

    return conn->prepare(query);
  }

  Statement Connection::prepareCached(const std::string& query, const std::string& key)
  {
    log_trace("Connection::prepareCached(\"" << query << "\")");

    return conn->prepareCached(query, key);
  }

  Statement IStmtCacheConnection::prepareCached(const std::string& query, const std::string& key)
  {
    log_trace("IStmtCacheConnection::prepare(\"" << query << ", " << key << "\")");

    stmtCacheType::iterator it = stmtCache.find(key);
    if (it == stmtCache.end())
    {
      log_debug("statement for query \"" << key << "\" not found in cache");
      Statement stmt = prepare(query);
      IStatement* istmt = const_cast<IStatement*>(stmt.getImpl());
      stmtCache.insert(stmtCacheType::value_type(key, istmt));
      return stmt;
    }
    else
    {
      log_debug("statement for query \"" << key << "\" fetched from cache");
      return Statement(it->second);
    }
  }

  void IStmtCacheConnection::clearStatementCache()
  {
    log_trace("IStmtCacheConnection::clearStatementCache()");

    stmtCache.clear();
  }
}
