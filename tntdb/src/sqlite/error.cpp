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

#include <tntdb/sqlite/error.h>
#include <cstdlib>

namespace tntdb
{
  namespace sqlite
  {
    SqliteError::SqliteError(const char* function, char* errmsg, bool do_free)
      : Error(std::string(function) + ": " + (errmsg ? errmsg : "unknown error"))
    {
      if (errmsg && do_free)
// I would like to use a autoconf here, but sqlite3_malloc/sqlite3_free did
// exist prior version 3.5 but was not used, so a configure-check is difficult.
// (at least I have no idea how to do it)
#if SQLITE_VERSION_NUMBER >= 3005000
        sqlite3_free(errmsg);
#else
        std::free(errmsg);
#endif
    }

    Execerror::Execerror(const char* function, sqlite3* db, int _errcode)
      : SqliteError(function, sqlite3_errmsg(db)),
        errcode(_errcode)
    { }

    Execerror::Execerror(const char* function, sqlite3_stmt* stmt, int _errcode)
      : SqliteError(function, sqlite3_errmsg(sqlite3_db_handle(stmt))),
        errcode(_errcode)
    { }
  }
}
