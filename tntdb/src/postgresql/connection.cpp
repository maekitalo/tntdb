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

#include <tntdb/postgresql/impl/connection.h>
#include <tntdb/postgresql/impl/result.h>
#include <tntdb/postgresql/impl/statement.h>
#include <tntdb/postgresql/error.h>
#include <tntdb/result.h>
#include <tntdb/statement.h>
#include <cxxtools/log.h>
#include <new>
#include <sstream>

log_define("tntdb.postgresql.connection");

namespace tntdb
{
  namespace postgresql
  {
    Connection::Connection(const char* conninfo)
    {
      log_debug("PQconnectdb(\"" << conninfo << "\")");

      conn = PQconnectdb(conninfo);
      if (conn == 0)
        throw std::bad_alloc();

      if (PQstatus(conn) == CONNECTION_BAD )
        throw PgConnError("PQconnect", conn);
    }

    Connection::~Connection()
    {
      if (conn)
      {
        clearStatementCache();

        log_debug("PQfinish(" << conn << ")");
        PQfinish(conn);
      }
    }

    void Connection::beginTransaction()
    {
      execute("BEGIN");
    }

    void Connection::commitTransaction()
    {
      execute("COMMIT");
    }

    void Connection::rollbackTransaction()
    {
      execute("ROLLBACK");
    }

    Connection::size_type Connection::execute(const std::string& query)
    {
      log_debug("execute(\"" << query << "\")");

      log_debug("PQexec(" << conn << ", \"" << query << "\")");
      PGresult* result = PQexec(conn, query.c_str());
      if (isError(result))
      {
        log_error(PQresultErrorMessage(result));
        throw PgSqlError(query, "PQexec", result, true);
      }

      std::istringstream tuples(PQcmdTuples(result));
      Connection::size_type ret = 0;
      tuples >> ret;

      log_debug("PQclear(" << result << ')');
      PQclear(result);

      return ret;
    }

    tntdb::Result Connection::select(const std::string& query)
    {
      log_debug("select(\"" << query << "\")");

      log_debug("PQexec(" << conn << ", \"" << query << "\")");
      PGresult* result = PQexec(conn, query.c_str());
      if (isError(result))
      {
        log_error(PQresultErrorMessage(result));
        throw PgSqlError(query, "PQexec", result, true);
      }

      return tntdb::Result(new Result(tntdb::Connection(this), result));
    }

    Row Connection::selectRow(const std::string& query)
    {
      log_debug("selectRow(\"" << query << "\")");
      tntdb::Result result = select(query);
      if (result.empty())
        throw NotFound();

      return result.getRow(0);
    }

    Value Connection::selectValue(const std::string& query)
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
