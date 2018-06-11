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

#include <tntdb/odbc/row.h>
#include <tntdb/odbc/error.h>
#include <tntdb/odbc/value.h>
#include <tntdb/value.h>

#include <cxxtools/log.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

log_define("tntdb.odbc.row")

namespace tntdb
{
namespace odbc
{
Row::Row(SQLHSTMT hStmt)
{
    SQLRETURN retval;

    SQLSMALLINT numCols;
    retval = SQLNumResultCols(hStmt, &numCols);
    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLNumResultCols failed", retval, SQL_HANDLE_STMT, hStmt);

    _values.resize(numCols);
    for (SQLSMALLINT n = 0; n < numCols; ++n)
        _values[n] = tntdb::Value(new odbc::Value(hStmt, n + 1));
}

Row::size_type Row::size() const
{
    return _values.size();
}

tntdb::Value Row::getValueByNumber(size_type field_num) const
{
    return _values[field_num];
}

tntdb::Value Row::getValueByName(const std::string& field_name) const
{
    throw std::runtime_error("tntdb::odbc::Row::getValueByName not implemented yet");
}

std::string Row::getColumnName(size_type field_num) const
{
    throw std::runtime_error("tntdb::odbc::Row::getColumnName not implemented yet");
}


}

}
