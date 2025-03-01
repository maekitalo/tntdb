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

#include <tntdb/sqlite/impl/cursor.h>
#include <tntdb/sqlite/impl/stmtrow.h>
#include <tntdb/sqlite/error.h>
#include <tntdb/row.h>
#include <cxxtools/log.h>

log_define("tntdb.sqlite.cursor")

namespace tntdb
{
namespace sqlite
{
Cursor::Cursor(Statement* stmt)
  : _stmt(stmt)
{ }

Row Cursor::fetch()
{
    _stmt->_needReset = true;
    sqlite3_stmt* stmt = _stmt->_stmt;

    log_debug("sqlite3_step(" << stmt << ')');
    int ret = ::sqlite3_step(stmt);
    if (ret == SQLITE_DONE)
        return Row();
    else if (ret != SQLITE_ROW)
        throw Execerror("sqlite3_step", stmt, ret);

    return Row(std::make_shared<StmtRow>(stmt));
}
}
}
