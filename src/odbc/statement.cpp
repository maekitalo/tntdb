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

#include <tntdb/odbc/statement.h>
#include <tntdb/odbc/connection.h>

#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <tntdb/error.h>

#include <cxxtools/log.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

log_define("tntdb.odbc.statement")

namespace tntdb
{
namespace odbc
{

Statement::Statement(Connection* conn, const std::string& query)
    : _conn(conn),
      _hStmt(SQL_HANDLE_STMT, conn->handle())
{
    // TODO
}

Statement::~Statement()
{
}

void Statement::clear()
{
}

void Statement::setNull(const std::string& col)
{
}

void Statement::setBool(const std::string& col, bool data)
{
}

void Statement::setShort(const std::string& col, short data)
{
}

void Statement::setInt(const std::string& col, int data)
{
}

void Statement::setLong(const std::string& col, long data)
{
}

void Statement::setUnsignedShort(const std::string& col, unsigned short data)
{
}

void Statement::setUnsigned(const std::string& col, unsigned data)
{
}

void Statement::setUnsignedLong(const std::string& col, unsigned long data)
{
}

void Statement::setInt32(const std::string& col, int32_t data)
{
}

void Statement::setUnsigned32(const std::string& col, uint32_t data)
{
}

void Statement::setInt64(const std::string& col, int64_t data)
{
}

void Statement::setUnsigned64(const std::string& col, uint64_t data)
{
}

void Statement::setDecimal(const std::string& col, const Decimal& data)
{
}

void Statement::setFloat(const std::string& col, float data)
{
}

void Statement::setDouble(const std::string& col, double data)
{
}

void Statement::setChar(const std::string& col, char data)
{
}

void Statement::setString(const std::string& col, const std::string& data)
{
}

void Statement::setBlob(const std::string& col, const Blob& data)
{
}

void Statement::setDate(const std::string& col, const Date& data)
{
}

void Statement::setTime(const std::string& col, const Time& data)
{
}

void Statement::setDatetime(const std::string& col, const Datetime& data)
{
}

Statement::size_type Statement::execute()
{
    // TODO
    return 0;
}

tntdb::Result Statement::select()
{
    // TODO
    return tntdb::Result();
}

tntdb::Row Statement::selectRow()
{
    // TODO
    return tntdb::Row();
}

tntdb::Value Statement::selectValue()
{
    // TODO
    return tntdb::Value();
}

ICursor* Statement::createCursor(unsigned fetchsize)
{
    // TODO
    throw Error("createCursor not implemented yet");
}

}
}
