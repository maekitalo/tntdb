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

#include <tntdb/odbc/value.h>
#include <tntdb/odbc/error.h>
#include <tntdb/value.h>

#include <cxxtools/convert.h>
#include <cxxtools/log.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

log_define("tntdb.odbc.value")

namespace tntdb
{
namespace odbc
{
Value::Value(SQLHSTMT hStmt, SQLUSMALLINT columnNumber)
{
    SQLRETURN retval;

    std::vector<SQLCHAR> columnName;
    SQLSMALLINT nameLength;
    SQLSMALLINT dataType;
    SQLULEN columnSize;
    SQLSMALLINT decimalDigits;
    SQLSMALLINT nullable;

    log_debug("SQLDescribeCol");
    retval = SQLDescribeCol(hStmt, columnNumber, &columnName[0], columnName.size(), &nameLength,
        &dataType, &columnSize, &decimalDigits, &nullable);
    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLDescribeCol failed", retval, SQL_HANDLE_STMT, hStmt);

    if (nameLength >= static_cast<SQLSMALLINT>(columnName.size()))
    {
        columnName.resize(nameLength + 1);
        log_debug("SQLDescribeCol");
        retval = SQLDescribeCol(hStmt, columnNumber, &columnName[0], columnName.size(), &nameLength,
            &dataType, &columnSize, &decimalDigits, &nullable);
        if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
            throw Error("SQLDescribeCol failed", retval, SQL_HANDLE_STMT, hStmt);
    }

    log_debug("name: \"" << &columnName[0] << "\" dataType: " << dataType
        << " columnSize: " << columnSize << " decimalDigits: " << decimalDigits
        << " nullable: " << nullable);

    _data.resize(columnSize + 20);

    retval = SQLBindCol(hStmt, columnNumber, SQL_C_CHAR, &_data[0], _data.size(), &_lenOrInd);
    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLBindCol failed", retval, SQL_HANDLE_STMT, hStmt);
}

bool Value::isNull() const
{
    return _lenOrInd == SQL_NULL_DATA;
}

bool Value::getBool() const
{
    return cxxtools::convert<bool>(&_data[0]);
}

short Value::getShort() const
{
    return cxxtools::convert<short>(&_data[0]);
}

int Value::getInt() const
{
    log_debug("getInt; " << _data.size());
    log_debug("getInt; " << &_data[0]);
    return cxxtools::convert<int>(&_data[0]);
}

long Value::getLong() const
{
    return cxxtools::convert<long>(&_data[0]);
}

unsigned Value::getUnsigned() const
{
    return cxxtools::convert<unsigned>(&_data[0]);
}

unsigned short Value::getUnsignedShort() const
{
    return cxxtools::convert<unsigned short>(&_data[0]);
}

unsigned long Value::getUnsignedLong() const
{
    return cxxtools::convert<unsigned long>(&_data[0]);
}

int32_t Value::getInt32() const
{
    return cxxtools::convert<int32_t>(&_data[0]);
}

uint32_t Value::getUnsigned32() const
{
    return cxxtools::convert<uint32_t>(&_data[0]);
}

int64_t Value::getInt64() const
{
    return cxxtools::convert<int64_t>(&_data[0]);
}

uint64_t Value::getUnsigned64() const
{
    return cxxtools::convert<uint64_t>(&_data[0]);
}

Decimal Value::getDecimal() const
{
}

float Value::getFloat() const
{
}

double Value::getDouble() const
{
}

char Value::getChar() const
{
}

void Value::getString(std::string& ret) const
{
}

void Value::getBlob(Blob& ret) const
{
}

Date Value::getDate() const
{
}

Time Value::getTime() const
{
}

Datetime Value::getDatetime() const
{
}

void Value::getUString(cxxtools::String& ret) const
{
}

}
}
