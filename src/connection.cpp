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

    _conn->beginTransaction();
  }

  void Connection::commitTransaction()
  {
    log_trace("Connection::commitTransaction");

    _conn->commitTransaction();
  }

  void Connection::rollbackTransaction()
  {
    log_trace("Connection::rollbackTransaction");

    _conn->rollbackTransaction();
  }

  Connection::size_type Connection::execute(const std::string& query)
  {
    log_trace("Connection::execute(\"" << query << "\")");

    return _conn->execute(query);
  }

  Result Connection::select(const std::string& query)
  {
    log_trace("Connection::select(\"" << query << "\")");

    return _conn->select(query);
  }

  Row Connection::selectRow(const std::string& query)
  {
    log_trace("Connection::selectRow(\"" << query << "\")");

    return _conn->selectRow(query);
  }

  Value Connection::selectValue(const std::string& query)
  {
    log_trace("Connection::selectValue(\"" << query << "\")");

    return _conn->selectValue(query);
  }

  Statement Connection::prepare(const std::string& query)
  {
    log_trace("Connection::prepare(\"" << query << "\")");

    return _conn->prepare(query);
  }

  Statement Connection::prepareWithLimit(const std::string& query, const std::string& limit, const std::string& offset)
  {
    log_trace("Connection::prepareWithLimit(\"" << query << ", " << limit << "\", \"" << offset << "\")");

    return _conn->prepareWithLimit(query, limit, offset);
  }

  Statement Connection::prepareCached(const std::string& query, const std::string& key)
  {
    log_trace("Connection::prepareCached(\"" << query << "\")");

    return _conn->prepareCached(query, key);
  }

  Statement Connection::prepareCachedWithLimit(const std::string& query, const std::string& limit, const std::string& offset, const std::string& key)
  {
    log_trace("Connection::prepareCachedWithLimit(\"" << query << ", " << limit << "\", \"" << offset << "\", \"" << key << "\")");

    return _conn->prepareCachedWithLimit(query, limit, offset, key);
  }

  std::string IConnection::url(const std::string& url, const std::string& username, const std::string& password)
  {
    enum {
      state_0,
      state_p,
      state_e
    } state = state_0;

    std::string ret;
    for (std::string::const_iterator it = url.begin(); it != url.end(); ++it)
    {
      char ch = *it;
      switch (state)
      {
        case state_0:
          if (ch == '%')
            state = state_p;
          else if (ch == '\\')
            state = state_e;
          else
            ret += ch;
          break;

        case state_p:
          if (ch == 'u')
          {
            ret += username;
            state = state_0;
          }
          else if (ch == 'p')
          {
            ret += password;
            state = state_0;
          }
          else if (ch == '%')
            ret += '%';
          else if (ch == '\\')
          {
            ret += '%';
            state = state_e;
          }
          else
          {
            ret += '%';
            ret += ch;
            state = state_0;
          }
          break;

        case state_e:
          ret += ch;
          state = state_0;
          break;
      }
    }

    switch (state)
    {
      case state_0:
        break;
      case state_p:
        ret += '%'; break;
      case state_e:
        ret += '\\'; break;
    }

    return ret;
  }

  Statement IStmtCacheConnection::prepareCached(const std::string& query, const std::string& key)
  {
    log_trace("IStmtCacheConnection::prepareCached(\"" << query << ", " << key << "\")");

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
      return Statement(it->second.getPointer());
    }
  }

  Statement IStmtCacheConnection::prepareCachedWithLimit(const std::string& query, const std::string& limit, const std::string& offset, const std::string& key)
  {
    log_trace("IStmtCacheConnection::prepareCachedWithLimit(\"" << query << ", " << limit << "\", \"" << offset << "\", \"" << key << "\")");

    std::string lkey = key + ':' + limit + ':' + offset;
    stmtCacheType::iterator it = stmtCache.find(lkey);
    if (it == stmtCache.end())
    {
      log_debug("statement for query \"" << lkey << "\" not found in cache");
      Statement stmt = prepareWithLimit(query, limit, offset);
      IStatement* istmt = const_cast<IStatement*>(stmt.getImpl());
      stmtCache.insert(stmtCacheType::value_type(lkey, istmt));
      return stmt;
    }
    else
    {
      log_debug("statement for query \"" << lkey << "\" fetched from cache");
      return Statement(it->second.getPointer());
    }
  }

  void IStmtCacheConnection::clearStatementCache()
  {
    log_trace("IStmtCacheConnection::clearStatementCache()");

    stmtCache.clear();
  }

  bool IStmtCacheConnection::clearStatementCache(const std::string& key)
  {
    log_trace("IStmtCacheConnection::clearStatementCache(\"" << key << "\")");

    stmtCacheType::iterator it = stmtCache.find(key);
    if (it == stmtCache.end())
      return  false;

    log_debug("remove statement for query \"" << key << "\" from cache");
    stmtCache.erase(it);
    return true;
  }
}

