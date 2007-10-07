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

#ifndef TNTDB_SQLITE_IMPL_STMTROW_H
#define TNTDB_SQLITE_IMPL_STMTROW_H

#include <tntdb/iface/irow.h>
#include <tntdb/sqlite/impl/statement.h>

namespace tntdb
{
  namespace sqlite
  {
    /// Row of a statement
    class StmtRow : public IRow
    {
        sqlite3_stmt* stmt;

      public:
        StmtRow(sqlite3_stmt* stmt_)
          : stmt(stmt_)
          { }

        unsigned size() const;
        Value getValue(size_type field_num) const;

        // specific methods of sqlite-driver
        sqlite3_stmt* getStmt() const   { return stmt; }
    };
  }
}

#endif // TNTDB_SQLITE_IMPL_STMTROW_H

