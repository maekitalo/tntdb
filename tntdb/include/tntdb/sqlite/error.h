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

