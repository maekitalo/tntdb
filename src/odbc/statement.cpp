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
#include <tntdb/odbc/value.h>
#include <tntdb/odbc/row.h>
#include <tntdb/odbc/result.h>
#include <tntdb/odbc/cursor.h>

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

char* Bind::ptr(size_t size)
{
    if (size <= _allocatedSize)
        return _u.ptr;

    releaseBuffer();

    if (size <= sizeof(_u))
    {
        return reinterpret_cast<char*>(&_u);
    }
    else
    {
        _u.ptr = new char[size];
        _allocatedSize = size;
        return _u.ptr;
    }
}

void Bind::releaseBuffer()
{
    if (_allocatedSize > 0)
    {
        delete[] _u.ptr;
        _allocatedSize = 0;
    }
}

Bind& Statement::getBind(const std::string& col) const
{
    BindMap::const_iterator it = _binds.find(col);
    if (it == _binds.end())
    {
        log_warn("hostvar \"" << col << "\" not found");
        throw tntdb::Error("hostvar \"" + col + "\" not found");
    }

    return *it->second;
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
    Bind& bind = getBind(col);
}

void Statement::setBool(const std::string& col, bool data)
{
    Bind& bind = getBind(col);
}

void Statement::setShort(const std::string& col, short data)
{
    log_debug("setShort(\"" << col << "\", " << data << ')');

    SQLRETURN retval;

    Bind& bind = getBind(col);
    short& shortValue = bind.shortValue();

    if (bind.boundType != SQL_C_SHORT)
    {
        for (unsigned n = 0; n < bind.colnums.size(); ++n)
        {
            log_debug("SQLBindParameter(hStmt, " << bind.colnums[n]+1 << ", SQL_PARAM_INPUT, SQL_C_SHORT, SQLINTEGER, 0, 0, ptr, " << sizeof(long) << ", 0)");
            retval = SQLBindParameter(_hStmt,
                bind.colnums[n] + 1,
                SQL_PARAM_INPUT,
                SQL_C_SHORT,
                SQL_INTEGER,
                0, // ColumnSize
                0, // DecimalDigits
                &shortValue,
                sizeof(short),
                0);

            if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO && retval != SQL_NO_DATA)
            {
                bind.boundType = SQL_UNKNOWN_TYPE;
                throw Error("SQLBindParameter(" + col + ") failed", retval, SQL_HANDLE_STMT, _hStmt);
            }

        }

        bind.boundType = SQL_C_SHORT;
    }

    for (unsigned n = 0; n < bind.colnums.size(); ++n)
        shortValue = data;
}

void Statement::setInt(const std::string& col, int data)
{
    log_debug("setInt(\"" << col << "\", " << data << ')');
    setLong(col, data);
}

void Statement::setLong(const std::string& col, long data)
{
    log_debug("setLong(\"" << col << "\", " << data << ')');

    SQLRETURN retval;

    Bind& bind = getBind(col);
    long& longValue = bind.longValue();

    if (bind.boundType != SQL_C_SLONG)
    {
        for (unsigned n = 0; n < bind.colnums.size(); ++n)
        {
            log_debug("SQLBindParameter(hStmt, " << bind.colnums[n]+1 << ", SQL_PARAM_INPUT, SQL_C_SLONG, SQLINTEGER, 0, 0, ptr, " << sizeof(long) << ", 0)");
            retval = SQLBindParameter(_hStmt,
                bind.colnums[n] + 1,
                SQL_PARAM_INPUT,
                SQL_C_SLONG,
                SQL_INTEGER,
                0, // ColumnSize
                0, // DecimalDigits
                &longValue,
                sizeof(long),
                0);

            if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO && retval != SQL_NO_DATA)
            {
                bind.boundType = SQL_UNKNOWN_TYPE;
                throw Error("SQLBindParameter(" + col + ") failed", retval, SQL_HANDLE_STMT, _hStmt);
            }

        }

        bind.boundType = SQL_C_SLONG;
    }

    for (unsigned n = 0; n < bind.colnums.size(); ++n)
        longValue = data;
}

void Statement::setUnsignedShort(const std::string& col, unsigned short data)
{
    log_debug("setUnsignedShort(\"" << col << "\", " << data << ')');

    SQLRETURN retval;

    Bind& bind = getBind(col);
    unsigned short& ushortValue = bind.ushortValue();

    if (bind.boundType != SQL_C_USHORT)
    {
        for (unsigned n = 0; n < bind.colnums.size(); ++n)
        {
            log_debug("SQLBindParameter(hStmt, " << bind.colnums[n]+1 << ", SQL_PARAM_INPUT, SQL_C_USHORT, SQLINTEGER, 0, 0, ptr, " << sizeof(long) << ", 0)");
            retval = SQLBindParameter(_hStmt,
                bind.colnums[n] + 1,
                SQL_PARAM_INPUT,
                SQL_C_USHORT,
                SQL_INTEGER,
                0, // ColumnSize
                0, // DecimalDigits
                &ushortValue,
                sizeof(unsigned short),
                0);

            if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO && retval != SQL_NO_DATA)
            {
                bind.boundType = SQL_UNKNOWN_TYPE;
                throw Error("SQLBindParameter(" + col + ") failed", retval, SQL_HANDLE_STMT, _hStmt);
            }

        }

        bind.boundType = SQL_C_USHORT;
    }

    for (unsigned n = 0; n < bind.colnums.size(); ++n)
        ushortValue = data;
}

void Statement::setUnsigned(const std::string& col, unsigned data)
{
    log_debug("setUnsigned(\"" << col << "\", " << data << ')');
    setUnsignedLong(col, data);
}

void Statement::setUnsignedLong(const std::string& col, unsigned long data)
{
    log_debug("setUnsignedLong(\"" << col << "\", " << data << ')');

    SQLRETURN retval;

    Bind& bind = getBind(col);
    unsigned long& ulongValue = bind.ulongValue();

    if (bind.boundType != SQL_C_ULONG)
    {
        for (unsigned n = 0; n < bind.colnums.size(); ++n)
        {
            log_debug("SQLBindParameter(hStmt, " << bind.colnums[n]+1 << ", SQL_PARAM_INPUT, SQL_C_ULONG, SQLINTEGER, 0, 0, ptr, " << sizeof(unsigned long) << ", 0)");
            retval = SQLBindParameter(_hStmt,
                bind.colnums[n] + 1,
                SQL_PARAM_INPUT,
                SQL_C_ULONG,
                SQL_INTEGER,
                0, // ColumnSize
                0, // DecimalDigits
                &ulongValue,
                sizeof(unsigned long),
                0);

            if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO && retval != SQL_NO_DATA)
            {
                bind.boundType = SQL_UNKNOWN_TYPE;
                throw Error("SQLBindParameter(" + col + ") failed", retval, SQL_HANDLE_STMT, _hStmt);
            }

        }

        bind.boundType = SQL_C_ULONG;
    }

    for (unsigned n = 0; n < bind.colnums.size(); ++n)
        ulongValue = data;
}

void Statement::setInt32(const std::string& col, int32_t data)
{
    setLong(col, data);
}

void Statement::setUnsigned32(const std::string& col, uint32_t data)
{
    setUnsignedLong(col, data);
}

void Statement::setInt64(const std::string& col, int64_t data)
{
    setLong(col, data);
}

void Statement::setUnsigned64(const std::string& col, uint64_t data)
{
    setUnsignedLong(col, data);
}

void Statement::setDecimal(const std::string& col, const Decimal& data)
{
}

void Statement::setFloat(const std::string& col, float data)
{
    log_debug("setFloat(\"" << col << "\", " << data << ')');

    SQLRETURN retval;

    Bind& bind = getBind(col);
    float& floatValue = bind.floatValue();

    if (bind.boundType != SQL_C_FLOAT)
    {
        for (unsigned n = 0; n < bind.colnums.size(); ++n)
        {
            log_debug("SQLBindParameter(hStmt, " << bind.colnums[n]+1 << ", SQL_PARAM_INPUT, SQL_C_FLOAT, SQLINTEGER, 0, 0, ptr, " << sizeof(float) << ", 0)");
            retval = SQLBindParameter(_hStmt,
                bind.colnums[n] + 1,
                SQL_PARAM_INPUT,
                SQL_C_FLOAT,
                SQL_FLOAT,
                0, // ColumnSize
                0, // DecimalDigits
                &floatValue,
                sizeof(float),
                0);

            if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO && retval != SQL_NO_DATA)
            {
                bind.boundType = SQL_UNKNOWN_TYPE;
                throw Error("SQLBindParameter(" + col + ") failed", retval, SQL_HANDLE_STMT, _hStmt);
            }

        }

        bind.boundType = SQL_C_FLOAT;
    }

    for (unsigned n = 0; n < bind.colnums.size(); ++n)
        floatValue = data;
}

void Statement::setDouble(const std::string& col, double data)
{
    log_debug("setDouble(\"" << col << "\", " << data << ')');

    SQLRETURN retval;

    Bind& bind = getBind(col);
    double& doubleValue = bind.doubleValue();

    if (bind.boundType != SQL_C_DOUBLE)
    {
        for (unsigned n = 0; n < bind.colnums.size(); ++n)
        {
            log_debug("SQLBindParameter(hStmt, " << bind.colnums[n]+1 << ", SQL_PARAM_INPUT, SQL_C_DOUBLE, SQLINTEGER, 0, 0, ptr, " << sizeof(long) << ", 0)");
            retval = SQLBindParameter(_hStmt,
                bind.colnums[n] + 1,
                SQL_PARAM_INPUT,
                SQL_C_DOUBLE,
                SQL_DOUBLE,
                0, // ColumnSize
                0, // DecimalDigits
                &doubleValue,
                sizeof(double),
                0);

            if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO && retval != SQL_NO_DATA)
            {
                bind.boundType = SQL_UNKNOWN_TYPE;
                throw Error("SQLBindParameter(" + col + ") failed", retval, SQL_HANDLE_STMT, _hStmt);
            }

        }

        bind.boundType = SQL_C_FLOAT;
    }

    for (unsigned n = 0; n < bind.colnums.size(); ++n)
        doubleValue = data;
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

    log_debug("SQLExecute(" << _hStmt << ')');
    retval = SQLExecute(_hStmt);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO && retval != SQL_NO_DATA)
        throw Error("SQLExecute failed", retval, SQL_HANDLE_STMT, _hStmt);

    log_debug("SQLRowCount");
    SQLLEN rowCount;
    retval = SQLRowCount(_hStmt, &rowCount);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLRowCount failed", retval, SQL_HANDLE_STMT, _hStmt);

    return static_cast<Statement::size_type>(rowCount);
}

tntdb::Result Statement::select()
{
    return tntdb::Result(new Result(_hStmt));
}

tntdb::Row Statement::selectRow()
{
    SQLRETURN retval;

    log_debug("SQLExecute(" << _hStmt << ')');
    retval = SQLExecute(_hStmt);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO && retval != SQL_NO_DATA)
        throw Error("SQLExecute failed", retval, SQL_HANDLE_STMT, _hStmt);

    tntdb::Row ret(new odbc::Row(_hStmt));

    log_debug("SQLFetch");
    retval = SQLFetch(_hStmt);
    if (retval == SQL_NO_DATA)
        throw NotFound();

    retval = SQLFreeStmt(_hStmt, SQL_CLOSE);
    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLFreeStmt failed", retval, SQL_HANDLE_STMT, _hStmt);

    return ret;
}

tntdb::Value Statement::selectValue()
{
    SQLRETURN retval;

    log_debug("SQLExecute(" << _hStmt << ')');
    retval = SQLExecute(_hStmt);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO && retval != SQL_NO_DATA)
        throw Error("SQLExecute failed", retval, SQL_HANDLE_STMT, _hStmt);

    tntdb::Value ret(new odbc::Value(_hStmt, 1));

    log_debug("SQLFetch");
    retval = SQLFetch(_hStmt);
    if (retval == SQL_NO_DATA)
        throw NotFound();

    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLFetch failed", retval, SQL_HANDLE_STMT, _hStmt);

    retval = SQLFreeStmt(_hStmt, SQL_CLOSE);
    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLFreeStmt failed", retval, SQL_HANDLE_STMT, _hStmt);

    return ret;
}

ICursor* Statement::createCursor(unsigned fetchsize)
{
    return new Cursor(_hStmt);
}

}
}
