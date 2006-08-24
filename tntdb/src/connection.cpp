/* 
 * Copyright (C) 2005 Tommi Maekitalo
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
