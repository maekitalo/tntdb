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

#include <tntdb/postgresql/impl/cursor.h>
#include <tntdb/postgresql/impl/connection.h>
#include <tntdb/postgresql/error.h>
#include <tntdb/bits/row.h>
#include <cxxtools/log.h>
#include <sstream>

log_define("tntdb.postgresql.cursor")

namespace tntdb
{
  namespace postgresql
  {
    Cursor::Cursor(Statement* statement, unsigned fetchSize_)
      : tntdbStmt(statement),
        stmt(statement),
        fetchSize(fetchSize_)
    { }

    Cursor::~Cursor()
    {
      if (!cursorName.empty())
      {
        std::string sql = "CLOSE " + cursorName;

        log_debug("PQexec(" << getPGConn() << ", \"" << sql << "\")");
        PGresult* result = PQexec(getPGConn(), sql.c_str());

        if (isError(result))
          log_error("error closing cursor: " << PQresultErrorMessage(result));

        log_debug("PQclear(" << result << ')');
        PQclear(result);
      }
    }

    Row Cursor::fetch()
    {
      if (cursorName.empty())
      {
        // create cursorname
        std::ostringstream s;
        s << "tntdbcur" << this;

        std::string sql = "DECLARE " + s.str()
          + " CURSOR WITH HOLD FOR "
          + stmt->getQuery();

        // declare cursor
        log_debug("PQexecParams(" << getPGConn() << ", \"" << sql
          << "\", " << stmt->getNParams() << ", 0, paramValues, paramLengths, 0, 0)");
        PGresult* result = PQexecParams(getPGConn(), sql.c_str(),
          stmt->getNParams(), 0,
          stmt->getParamValues(), stmt->getParamLengths(),
          0, 0);

        if (isError(result))
        {
          log_error(PQresultErrorMessage(result));
          throw PgSqlError(sql, "PQexecParams", result, true);
        }

        log_debug("PQclear(" << result << ')');
        PQclear(result);

        cursorName = s.str();
      }

      if (!currentResult || currentRow >= currentResult.size())
      {
        log_debug("fetch cursor");

        std::ostringstream sql;
        sql << "FETCH " << fetchSize << " FROM " + cursorName;
        currentResult = stmt->getConnection()->select(sql.str());
        log_debug(currentResult.size() << " rows fetched");

        currentRow = 0;

        if (currentResult.empty())
          return Row();
      }

      return currentResult[currentRow++];
    }
  }
}
