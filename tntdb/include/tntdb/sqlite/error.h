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

#ifndef TNTDB_SQLITE_ERROR_H
#define TNTDB_SQLITE_ERROR_H

#include <tntdb/error.h>
#include <sqlite3.h>

namespace tntdb
{
  namespace sqlite
  {
    class SqliteError : public Error
    {
        const char* function;

      public:
        SqliteError(const char* function, const std::string& msg)
          : Error(std::string(function) + ": " + msg)
          { }
        SqliteError(const char* function, const char* errmsg)
          : Error(std::string(function) + ": " + (errmsg ? errmsg : "unknown error"))
          { }
        SqliteError(const char* function, char* errmsg, bool do_free);

        const char* getFunction() const   { return function; }
    };

    class Execerror : public SqliteError
    {
        int errcode;

      public:
        Execerror(const char* function, sqlite3* db, int _errcode);
        Execerror(const char* function, sqlite3_stmt* stmt, int _errcode);
        Execerror(const char* function, int _errcode, const char* errmsg)
          : SqliteError(function, errmsg),
            errcode(_errcode)
          { }
        Execerror(const char* function, int _errcode, char* errmsg, bool do_free)
          : SqliteError(function, errmsg, do_free),
            errcode(_errcode)
          { }

        int getErrorcode() const
          { return errcode; }
    };
  }
}

#endif // TNTDB_SQLITE_ERROR_H

