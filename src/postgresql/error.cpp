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

#include <tntdb/postgresql/error.h>
#include <sstream>
#include <cxxtools/log.h>

log_define("tntdb.postgresql.error")

namespace tntdb
{
  namespace postgresql
  {
    namespace
    {
      std::string errorMessage(const char* function, PGconn* conn)
      {
        std::ostringstream msg;
        msg << "Postgresql-Error in " << function << ": " << PQerrorMessage(conn);
        return msg.str();
      }

      std::string errorMessage(PGconn* conn)
      {
        std::ostringstream msg;
        msg << "Postgresql-Error: " << PQerrorMessage(conn);
        return msg.str();
      }

      std::string errorMessage(const char* function, const PGresult* res)
      {
        std::ostringstream msg;
        const char* sqlstate = PQresultErrorField(res, PG_DIAG_SQLSTATE);
        const char* primary = PQresultErrorField(res, PG_DIAG_MESSAGE_PRIMARY);
        const char* detail = PQresultErrorField(res, PG_DIAG_MESSAGE_DETAIL);
        const char* position = PQresultErrorField(res, PG_DIAG_STATEMENT_POSITION);
        msg << "Postgresql-Error " << sqlstate;
        if (primary)
          msg << ": " << primary;
        if (detail)
          msg << "; " << detail;
        if (position)
          msg << " at " << position;
        if (function)
          msg << " in " << function;
        return msg.str();
      }

      std::string errorMessage(const PGresult* res)
      {
        return errorMessage(0, res);
      }
    }

    PgConnError::PgConnError(PGconn* conn)
      : Error(errorMessage(conn))
      { }

    PgConnError::PgConnError(const char* function, PGconn* conn)
      : Error(errorMessage(function, conn))
      { }

    PgConnError::PgConnError(PGresult* result, bool free)
      : Error(errorMessage(result))
    {
      if (result && free)
      {
        log_debug("PQclear(" << result << ')');
        PQclear(result);
      }
    }

    PgConnError::PgConnError(const char* function, PGresult* result, bool free)
      : Error(errorMessage(function, result))
    {
      if (result && free)
      {
        log_debug("PQclear(" << result << ')');
        PQclear(result);
      }
    }

    PgSqlError::PgSqlError(const std::string& sql, PGconn* conn)
      : SqlError(sql, errorMessage(conn))
      { }

    PgSqlError::PgSqlError(const std::string& sql, const char* function, PGconn* conn)
      : SqlError(sql, errorMessage(function, conn))
      { }

    PgSqlError::PgSqlError(const std::string& sql, PGresult* result, bool free)
      : SqlError(sql, errorMessage(result))
    {
      if (result && free)
      {
        log_debug("PQclear(" << result << ')');
        PQclear(result);
      }
    }

    PgSqlError::PgSqlError(const std::string& sql, const char* function, PGresult* result, bool free)
      : SqlError(sql, errorMessage(function, result))
    {
      if (result && free)
      {
        log_debug("PQclear(" << result << ')');
        PQclear(result);
      }
    }
  }
}
