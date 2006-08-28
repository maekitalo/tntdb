/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#ifndef TNTDB_SQLITE_IMPL_CURSOR_H
#define TNTDB_SQLITE_IMPL_CURSOR_H

#include <tntdb/iface/icursor.h>
#include <tntdb/smartptr.h>
#include <sqlite3.h>

namespace tntdb
{
  namespace sqlite
  {
    class Statement;

    class Cursor : public ICursor
    {
        SmartPtr<Statement, InternalRefCounted> statement;
        sqlite3_stmt* stmt;

      public:
        Cursor(Statement* statement, sqlite3_stmt* stmt);
        ~Cursor();

        // method for ICursor
        Row fetch();

        // specific methods of sqlite-driver
        sqlite3_stmt* getStmt() const   { return stmt; }
    };
  }
}

#endif // TNTDB_SQLITE_IMPL_CURSOR_H

