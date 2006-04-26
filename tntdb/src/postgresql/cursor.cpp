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

#include <tntdb/postgresql/impl/cursor.h>
#include <tntdb/postgresql/impl/connection.h>
#include <tntdb/postgresql/error.h>
#include <tntdb/bits/row.h>
#include <cxxtools/log.h>
#include <sstream>

log_define("tntdb.postgresql.cursor");

namespace tntdb
{
  namespace postgresql
  {
    Cursor::Cursor(Statement* statement)
      : tntdbStmt(statement),
        stmt(statement),
        fetchSize(100)
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

        currentRow = 0;

        if (currentResult.empty())
          return Row();
      }

      return currentResult[currentRow++];
    }
  }
}
