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

#include <tntdb/postgresql/error.h>
#include <sstream>
#include <cxxtools/log.h>

log_define("tntdb.postgresql.error");

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
