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

