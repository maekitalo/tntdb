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

namespace
{
  int select_callback(void *pArg, int argc, char ** argv, char **columnNames)
  {
    log_debug("select_callback with " << argc << " columns");
    tntdb::ResultImpl* res = static_cast<tntdb::ResultImpl*>(pArg);

    tntdb::RowImpl::data_type data;

    for (int i = 0; i < argc; ++i)
    {
      tntdb::Value v;
      if (argv[i])
        v = tntdb::Value(new tntdb::ValueImpl(argv[i]));
      data.push_back(v);
    }

    res->add(tntdb::Row(new tntdb::RowImpl(data)));

    return SQLITE_OK;
  }
}


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
      char* errmsg;

      log_debug("sqlite3_exec(" << db << ", \"" << query << "\", 0, 0, " << &errmsg << ')');

      ResultImpl* r = new ResultImpl();
      Result result(r);
      int ret = ::sqlite3_exec(db, query.c_str(), select_callback, r, &errmsg);

      log_debug("sqlite3_exec ret=" << ret);

      if (ret != SQLITE_OK)
        throw Execerror("sqlite3_exec", ret, errmsg, true);

      return result;
    }

    tntdb::Row Connection::selectRow(const std::string& query)
    {
      log_debug("selectRow(\"" << query << "\")");
      tntdb::Result result = select(query);
      if (result.empty())
        throw NotFound();

      return result.getRow(0);
    }

    tntdb::Value Connection::selectValue(const std::string& query)
    {
      log_debug("selectValue(\"" << query << "\")");
      Row t = selectRow(query);
      if (t.empty())
        throw NotFound();

      return t.getValue(0);
    }

    tntdb::Statement Connection::prepare(const std::string& query)
    {
      log_debug("prepare(\"" << query << "\")");
      return tntdb::Statement(new Statement(this, query));
    }

  }
}
