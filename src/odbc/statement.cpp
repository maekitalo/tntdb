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
#include <tntdb/odbc/error.h>

#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <tntdb/error.h>
#include <tntdb/stmtparser.h>

#include <cxxtools/log.h>

#include <vector>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

log_define("tntdb.odbc.statement")

namespace tntdb
{
namespace odbc
{

namespace
{
    class SE : public StmtEvent
    {
        BindMap& _binds;
        unsigned _idx;
        static const std::string _hostvarInd;

    public:
        explicit SE(BindMap& binds)
            : _binds(binds),
              _idx(0)
            { }
        std::string onHostVar(const std::string& name);
        unsigned getCount() const  { return _idx; }
    };

    std::string SE::onHostVar(const std::string& name)
    {
        log_debug("hostvar :" << name << ", idx=" << _idx);

        BindMap::iterator it = _binds.find(name);
        if (it == _binds.end())
        {
            it = _binds.insert(BindMap::value_type(name, new Bind())).first;
        }

        it->second->colnums.push_back(_idx++);

        return _hostvarInd;
    }

    const std::string SE::_hostvarInd = "?";
}

Bind* Statement::getBind(const std::string& col) const
{
    BindMap::const_iterator it = _binds.find(col);
    if (it == _binds.end())
    {
        log_warn("hostvar \"" << col << "\" not found");
        return 0;
    }

    return it->second.getPointer();
}

Statement::Statement(Connection* conn, const std::string& query)
    : _conn(conn),
      _hStmt(SQL_HANDLE_STMT, SQL_HANDLE_DBC, conn->handle())
{
    // parse hostvars
    StmtParser parser;
    SE se(_binds);
    parser.parse(query, se);
    parser.getSql();

    SQLRETURN retval;

    retval = SQLPrepare(_hStmt,
        reinterpret_cast<SQLCHAR*>(const_cast<char*>((parser.getSql().data()))),
        parser.getSql().size());
    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLPrepare failed", retval, SQL_HANDLE_STMT, _hStmt);

    log_debug("sql=\"" << parser.getSql() << "\" invars " << se.getCount());

}

Statement::~Statement()
{
}

void Statement::clear()
{
    SQLRETURN retval = SQLFreeStmt(_hStmt, SQL_RESET_PARAMS);
    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLFreeStmt(handle, SQL_RESET_PARAMS) failed", retval, SQL_HANDLE_STMT, _hStmt);
}

void Statement::setNull(const std::string& col)
{
    Bind* bind = getBind(col);
    if (!bind)
        return;
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
    SQLRETURN retval;
    
    retval = SQLExecute(_hStmt);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLExecute failed", retval, SQL_HANDLE_STMT, _hStmt);

    SQLLEN rowCount;
    retval = SQLRowCount(_hStmt, &rowCount);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLRowCount failed", retval, SQL_HANDLE_STMT, _hStmt);

    return static_cast<Statement::size_type>(rowCount);
}

tntdb::Result Statement::select()
{
    SQLSMALLINT numCols;
    SQLNumResultCols(_hStmt, &numCols);

    log_debug(numCols << " columns");

    for (SQLSMALLINT c = 0; c < numCols; ++c)
    {
        std::vector<SQLCHAR> columnName;
        SQLSMALLINT nameLength;
        SQLSMALLINT dataType;
        SQLULEN columnSize;
        SQLSMALLINT decimalDigits;
        SQLSMALLINT nullable;

        SQLDescribeCol(_hStmt, c, &columnName[0], columnName.size(), &nameLength,
            &dataType, &columnSize, &decimalDigits, &nullable);

        if (nameLength >= static_cast<SQLSMALLINT>(columnName.size()))
        {
            columnName.resize(nameLength + 1);
            SQLDescribeCol(_hStmt, c, &columnName[0], columnName.size(), &nameLength,
                &dataType, &columnSize, &decimalDigits, &nullable);
        }

        log_debug("column " << c << " name: \"" << &columnName[0] << "\" dataType: " << dataType
            << " columnSize: " << columnSize << " decimalDigits: " << decimalDigits
            << " nullable: " << nullable);
    }
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
    throw std::runtime_error("tntdb::odbc::Statement::createCursor not implemented yet");
}

}
}
