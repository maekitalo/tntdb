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

#include <tntdb/sqlite/impl/cursor.h>
#include <tntdb/sqlite/impl/stmtrow.h>
#include <tntdb/sqlite/error.h>
#include <tntdb/row.h>
#include <cxxtools/log.h>

log_define("tntdb.sqlite.cursor");

namespace tntdb
{
  namespace sqlite
  {
    Cursor::Cursor(Statement* statement_, sqlite3_stmt* stmt_)
      : statement(statement_),
        stmt(stmt_)
    { }

    Cursor::~Cursor()
    {
      statement->putback(stmt);
    }

    Row Cursor::fetch()
    {
      log_debug("sqlite3_step(" << stmt << ')');
      int ret = ::sqlite3_step(stmt);
      if (ret == SQLITE_DONE)
        return Row();
      else if (ret != SQLITE_ROW)
        throw Execerror("sqlite3_step", stmt, ret);

      return Row(new StmtRow(getStmt()));
    }
  }
}
