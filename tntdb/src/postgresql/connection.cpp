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

#include <tntdb/postgresql/impl/connection.h>
#include <tntdb/postgresql/impl/result.h>
#include <tntdb/postgresql/impl/statement.h>
#include <tntdb/postgresql/error.h>
#include <tntdb/result.h>
#include <tntdb/statement.h>
#include <cxxtools/convert.h>
#include <cxxtools/log.h>
#include <new>
#include <poll.h>

log_define("tntdb.postgresql.connection")

namespace tntdb
{
  namespace postgresql
  {
    Connection::Connection(const char* conninfo)
      : transactionActive(0),
        stmtCounter(0)
    {
      log_debug("PQconnectdb(\"" << conninfo << "\")");

      conn = PQconnectdb(conninfo);
      if (conn == 0)
        throw std::bad_alloc();

      if (PQstatus(conn) == CONNECTION_BAD )
        throw PgConnError("PQconnectdb", conn);

      log_debug("connected to postgresql backend process " << PQbackendPID(conn));
    }

    Connection::~Connection()
    {
      if (conn)
      {
        clearStatementCache();
        currvalStmt = tntdb::Statement();

        log_debug("PQfinish(" << conn << ")");
        PQfinish(conn);
      }
    }

    void Connection::beginTransaction()
    {
      if (transactionActive == 0)
        execute("BEGIN");
      ++transactionActive;
    }

    void Connection::commitTransaction()
    {
      if (transactionActive == 0 || --transactionActive == 0)
      {
        execute("COMMIT");
        deallocateStatements();
      }
    }

    void Connection::rollbackTransaction()
    {
      if (transactionActive == 0 || --transactionActive == 0)
      {
        execute("ROLLBACK");
        deallocateStatements();
      }
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

      std::string t = PQcmdTuples(result);
      Connection::size_type ret = t.empty() ? 0
                                            : cxxtools::convert<Connection::size_type>(t);

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

    bool Connection::ping()
    {
      log_debug("ping()");

      if (PQsendQuery(conn, "select 1") == 0)
      {
        log_debug("failed to send statement \"select 1\" to database in Connection::ping()");
        return false;
      }

      while (true)
      {
        struct pollfd fd;
        fd.fd = PQsocket(conn);
        fd.events = POLLIN;
        log_debug("wait for input on fd " << fd.fd);
        if (::poll(&fd, 1, 10000) != 1)
        {
          log_debug("no data received in Connection::ping()");
          return false;
        }

        log_debug("consumeInput");
        if (PQconsumeInput(conn) == 0)
        {
          log_debug("PQconsumeInput failed in Connection::ping()");
          return false;
        }

        log_debug("check PQisBusy");
        while (PQisBusy(conn) == 0)
        {
          log_debug("PQgetResult");
          PGresult* result = PQgetResult(conn);

          log_debug("PQgetResult => " << static_cast<void*>(result));
          if (result == 0)
            return true;

          log_debug("PQfree");
          PQclear(result);
        }
      }
    }

    long Connection::lastInsertId(const std::string& name)
    {
      long ret = 0;

      if (name.empty())
      {
        if (!lastvalStmt)
          lastvalStmt = prepare("select lastval()");

        lastvalStmt
          .selectValue()
          .get(ret);
      }
      else
      {
        if (!currvalStmt)
          currvalStmt = prepare("select currval(:name)");

        currvalStmt.set("name", name)
          .selectValue()
          .get(ret);

      }

      return ret;
    }

    void Connection::deallocateStatement(const std::string& stmtName)
    {
      // Delay deallocation since a postgresql fail to execute anything including
      // deallocate statements when a failed transaction is active.

      stmtsToDeallocate.push_back(stmtName);
      if (transactionActive == 0)
        deallocateStatements();
    }

    void Connection::deallocateStatements()
    {
      for (std::vector<std::string>::size_type n = 0; n < stmtsToDeallocate.size(); ++n)
      {
        std::string sql = "DEALLOCATE " + stmtsToDeallocate[n];

        log_debug("PQexec(" << getPGConn() << ", \"" << sql << "\")");
        PGresult* result = PQexec(getPGConn(), sql.c_str());

        if (isError(result))
          log_error("error deallocating statement: " << PQresultErrorMessage(result));

        log_debug("PQclear(" << result << ')');
        PQclear(result);
      }

      stmtsToDeallocate.clear();
    }

    void Connection::lockTable(const std::string& tablename, bool exclusive)
    {
      std::string query = "LOCK TABLE ";
      query += tablename;
      query += exclusive ? " IN ACCESS EXCLUSIVE MODE" : " IN SHARE MODE";
      log_debug("execute(\"" << query << "\")");

      PGresult* result = PQexec(conn, query.c_str());
      if (isError(result))
      {
        log_error(PQresultErrorMessage(result));
        throw PgSqlError(query, "PQexec", result, true);
      }
    }
  }
}
