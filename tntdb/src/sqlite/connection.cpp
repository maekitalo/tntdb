/* 
 * Copyright (C) 2005 Tommi Maekitalo
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

#include <tntdb/sqlite/impl/connection.h>
#include <tntdb/sqlite/impl/statement.h>
#include <tntdb/impl/row.h>
#include <tntdb/impl/value.h>
#include <tntdb/impl/result.h>
#include <tntdb/sqlite/error.h>
#include <tntdb/bits/result.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/value.h>
#include <tntdb/bits/statement.h>
#include <cxxtools/log.h>

log_define("tntdb.sqlite.connection");

namespace tntdb
{
  namespace sqlite
  {
    Connection::Connection(const char* conninfo)
    {
      log_debug("sqlite3_open(\"" << conninfo << "\")");
      int errcode = ::sqlite3_open(conninfo, &db);

      if (db == 0)
        throw Execerror("sqlite3_open", db, errcode);

      log_debug("sqlite3 = " << db);
    }

    Connection::~Connection()
    {
      if (db)
      {
        clearStatementCache();

        log_debug("sqlite3_close(" << db << ")");
        ::sqlite3_close(db);
      }
    }

    void Connection::beginTransaction()
    {
      execute("BEGIN TRANSACTION");
    }

    void Connection::commitTransaction()
    {
      execute("COMMIT TRANSACTION");
    }

    void Connection::rollbackTransaction()
    {
      execute("ROLLBACK TRANSACTION");
    }

    Connection::size_type Connection::execute(const std::string& query)
    {
      char* errmsg;

      log_debug("sqlite3_exec(" << db << ", \"" << query << "\", 0, 0, " << &errmsg << ')');

      int ret = ::sqlite3_exec(db, query.c_str(), 0, 0, &errmsg);

      log_debug("sqlite3_exec ret=" << ret);

      if (ret != SQLITE_OK)
        throw Execerror("sqlite3_exec", ret, errmsg, true);

      return ::sqlite3_changes(db);
    }

    tntdb::Result Connection::select(const std::string& query)
    {
      return prepare(query).select();
    }

    tntdb::Row Connection::selectRow(const std::string& query)
    {
      return prepare(query).selectRow();
    }

    tntdb::Value Connection::selectValue(const std::string& query)
    {
      return prepare(query).selectValue();
    }

    tntdb::Statement Connection::prepare(const std::string& query)
    {
      log_debug("prepare(\"" << query << "\")");
      return tntdb::Statement(new Statement(this, query));
    }

  }
}
