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

#ifndef TNTDB_POSTGRESQL_ERROR_H
#define TNTDB_POSTGRESQL_ERROR_H

#include <tntdb/error.h>
#include <libpq-fe.h>

namespace tntdb
{
  namespace postgresql
  {
    class PgError
    {
      protected:
        PgError() { }
    };

    class PgConnError : public Error, public PgError
    {
      protected:
        PgConnError(const std::string& msg)
          : Error(msg)
          { }

      public:
        PgConnError(PGconn* conn);
        PgConnError(const char* function, PGconn* conn);
        PgConnError(PGresult* result, bool free);
        PgConnError(const char* function, PGresult* result, bool free);
    };

    class PgSqlError : public SqlError, public PgError
    {
      public:
        PgSqlError(const std::string& sql, PGconn* conn);
        PgSqlError(const std::string& sql, const char* function, PGconn* conn);
        PgSqlError(const std::string& sql, PGresult* result, bool free);
        PgSqlError(const std::string& sql, const char* function, PGresult* result, bool free);
    };

  }
}

#endif // TNTDB_POSTGRESQL_ERROR_H

