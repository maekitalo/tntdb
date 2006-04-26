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

#include <tntdb/connection.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <tntdb/statement.h>
#include <cxxtools/log.h>

log_define("tntdb.connection");

namespace tntdb
{
  void Connection::beginTransaction()
  {
    conn->beginTransaction();
  }

  void Connection::commitTransaction()
  {
    conn->commitTransaction();
  }

  void Connection::rollbackTransaction()
  {
    conn->rollbackTransaction();
  }

  Connection::size_type Connection::execute(const std::string& query)
  {
    return conn->execute(query);
  }

  Result Connection::select(const std::string& query)
  {
    return conn->select(query);
  }

  Row Connection::selectRow(const std::string& query)
  {
    return conn->selectRow(query);
  }

  Value Connection::selectValue(const std::string& query)
  {
    return conn->selectValue(query);
  }

  Statement Connection::prepare(const std::string& query)
  {
    return conn->prepare(query);
  }

  Statement Connection::prepareCached(const std::string& query)
  {
    return conn->prepareCached(query);
  }

  Statement IStmtCacheConnection::prepareCached(const std::string& query)
  {
    stmtCacheType::iterator it = stmtCache.find(query);
    if (it == stmtCache.end())
    {
      log_debug("statement for query \"" << query << "\" not found in cache");
      Statement stmt = prepare(query);
      IStatement* istmt = const_cast<IStatement*>(stmt.getImpl());
      stmtCache.insert(stmtCacheType::value_type(query, istmt));
      return stmt;
    }
    else
    {
      log_debug("statement for query \"" << query << "\" fetched from cache");
      return Statement(it->second);
    }
  }

  void IStmtCacheConnection::clearStatementCache()
  {
    stmtCache.clear();
  }
}
