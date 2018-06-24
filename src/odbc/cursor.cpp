/*
 * Copyright (C) 2018 Tommi Maekitalo
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

#include <tntdb/odbc/cursor.h>
#include <tntdb/odbc/row.h>
#include <tntdb/odbc/error.h>

#include <cxxtools/log.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

log_define("tntdb.odbc.cursor")

namespace tntdb
{
namespace odbc
{
Cursor::Cursor(SQLHSTMT hStmt)
    : _hStmt(hStmt)
{
    SQLRETURN retval;

    log_debug("SQLExecute");
    retval = SQLExecute(hStmt);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO && retval != SQL_NO_DATA)
        throw Error("SQLExecute failed", retval, SQL_HANDLE_STMT, hStmt);

    _row = tntdb::Row(new Row(hStmt));
}

Cursor::~Cursor()
{
    SQLRETURN retval;
    retval = SQLFreeStmt(_hStmt, SQL_CLOSE);
    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
    {
        log_error(Error("SQLFreeStmt failed", retval, SQL_HANDLE_STMT, _hStmt).what());
    }

}

tntdb::Row Cursor::fetch()
{
    SQLRETURN retval;

    log_debug("SQLFetch");
    retval = SQLFetch(_hStmt);
    if (retval == SQL_NO_DATA)
        return tntdb::Row();

    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO && retval != SQL_NO_DATA)
        throw Error("SQLFetch failed", retval, SQL_HANDLE_STMT, _hStmt);

    return _row;
}

}
}
