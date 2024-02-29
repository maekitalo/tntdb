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

#include <tntdb/sqlite/impl/statement.h>
#include <tntdb/sqlite/impl/cursor.h>
#include <tntdb/sqlite/impl/connection.h>
#include <tntdb/impl/result.h>
#include <tntdb/impl/row.h>
#include <tntdb/impl/value.h>
#include <tntdb/sqlite/error.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <sstream>
#include <limits>
#include <cxxtools/log.h>
#include "config.h"

log_define("tntdb.sqlite.statement")

namespace tntdb
{
namespace sqlite
{
Statement::Statement(Connection& conn, const std::string& query)
: _stmt(0),
  _conn(conn),
  _query(query),
  _needReset(false)
{
}

Statement::~Statement()
{
    if (_stmt)
    {
        log_debug("sqlite3_finalize(" << _stmt << ')');
        ::sqlite3_finalize(_stmt);
    }
}

sqlite3_stmt* Statement::getBindStmt()
{
    if (_stmt == 0)
    {
        // hostvars don't need to be parsed, because sqlite accepts the hostvar-
        // syntax of tntdb (:vvv)

        // prepare statement
        const char* tzTail;
#ifdef HAVE_SQLITE3_PREPARE_V2
        log_debug("sqlite3_prepare_v2(" << _conn.getSqlite3() << ", \"" << _query
          << "\", " << &_stmt << ", " << &tzTail << ')');
        int ret = ::sqlite3_prepare_v2(_conn.getSqlite3(), _query.data(), _query.size(), &_stmt, &tzTail);

        if (ret != SQLITE_OK)
            throw Execerror("sqlite3_prepare_v2", _conn.getSqlite3(), ret);
#else
        log_debug("sqlite3_prepare(" << _conn.getSqlite3() << ", \"" << _query
          << "\", " << &_stmt << ", " << &tzTail << ')');
        int ret = ::sqlite3_prepare(_conn.getSqlite3(), _query.data(), _query.size(), &_stmt, &tzTail);

        if (ret != SQLITE_OK)
            throw Execerror("sqlite3_prepare", _conn.getSqlite3(), ret);
#endif

        log_debug("sqlite3_stmt = " << _stmt);
    }
    else if (_needReset)
        reset();

    return _stmt;
}

int Statement::getBindIndex(const std::string& col)
{
    getBindStmt();

    log_debug("sqlite3_bind_parameter_index(" << _stmt << ", :" << col  << ')');
    int idx = ::sqlite3_bind_parameter_index(_stmt, (':' + col).c_str());
    if (idx == 0)
        log_warn("hostvariable :" << col << " not found");
    return idx;
}

void Statement::reset()
{
    if (_stmt)
    {
        if (_needReset)
        {
            log_debug("sqlite3_reset(" << _stmt << ')');
            int ret = ::sqlite3_reset(_stmt);

            if (ret != SQLITE_OK)
                throw Execerror("sqlite3_reset", _stmt, ret);

            _needReset = false;
        }
    }
    else
        getBindStmt();
}

void Statement::clear()
{
    getBindStmt();
    int count = ::sqlite3_bind_parameter_count(_stmt);
    for (int i = 0; i < count; ++i)
    {
        int ret = ::sqlite3_bind_null(_stmt, i + 1);
        if (ret != SQLITE_OK)
            throw Execerror("sqlite3_bind_null", _stmt, ret);
    }
}

void Statement::setNull(const std::string& col)
{
    int idx = getBindIndex(col);
    getBindStmt();
    if (idx != 0)
    {
        reset();

        log_debug("sqlite3_bind_null(" << _stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_null(_stmt, idx);

        if (ret != SQLITE_OK)
            throw Execerror("sqlite3_bind_null", _stmt, ret);
    }
}

void Statement::setBool(const std::string& col, bool data)
{
    setInt(col, data ? 1 : 0);
}

void Statement::setShort(const std::string& col, short data)
{
    setInt(col, data);
}

void Statement::setInt(const std::string& col, int data)
{
    int idx = getBindIndex(col);
    getBindStmt();
    if (idx != 0)
    {
        reset();

        log_debug("sqlite3_bind_int(" << _stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_int(_stmt, idx, data);

        if (ret != SQLITE_OK)
            throw Execerror("sqlite3_bind_int", _stmt, ret);
    }
}

void Statement::setLong(const std::string& col, long data)
{
    int idx = getBindIndex(col);
    getBindStmt();
    if (idx != 0)
    {
        reset();

        log_debug("sqlite3_bind_int64(" << _stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_int64(_stmt, idx, data);

        if (ret != SQLITE_OK)
            throw Execerror("sqlite3_bind_int", _stmt, ret);
    }
}

void Statement::setInt32(const std::string& col, int32_t data)
{
    setInt(col, data);
}

void Statement::setUnsignedShort(const std::string& col, unsigned short data)
{
    if (data > static_cast<unsigned short>(std::numeric_limits<unsigned short>::max()))
    {
        log_warn("possible loss of precision while converting unsigned short " << data
          << " to double");
        setDouble(col, static_cast<double>(data));
    }
    else
        setInt(col, static_cast<int>(data));
}

void Statement::setUnsigned(const std::string& col, unsigned data)
{
    if (data > static_cast<unsigned>(std::numeric_limits<int>::max()))
    {
        log_warn("possible loss of precision while converting unsigned " << data
          << " to double");
        setDouble(col, static_cast<double>(data));
    }
    else
        setInt(col, static_cast<int>(data));
}

void Statement::setUnsignedLong(const std::string& col, unsigned long data)
{
    if (data > static_cast<unsigned long>(std::numeric_limits<long>::max()))
    {
        log_warn("possible loss of precision while converting long unsigned " << data
          << " to double");
        setDouble(col, static_cast<double>(data));
    }
    else
        setLong(col, static_cast<long>(data));
}

void Statement::setUnsigned32(const std::string& col, uint32_t data)
{
    setUnsigned(col, data);
}

void Statement::setInt64(const std::string& col, int64_t data)
{
    int idx = getBindIndex(col);
    getBindStmt();
    if (idx != 0)
    {
        reset();

        log_debug("sqlite3_bind_int64(" << _stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_int64(_stmt, idx, data);

        if (ret != SQLITE_OK)
            throw Execerror("sqlite3_bind_int64", _stmt, ret);
    }
}

void Statement::setUnsigned64(const std::string& col, uint64_t data)
{
    setInt64(col, (int64_t)data);
}

void Statement::setDecimal(const std::string& col, const Decimal& data)
{
    // SQLite 3.4.1 does not support the SQL decimal or numeric types.
    // So double is used instead, but of course binary floating point can
    // not accurately store decimal floating point numbers.
    // Maybe we could try instead using:
    // int sqlite3_value_numeric_type(sqlite3_value*);
    // However I do not understand the SQLite 3.4.1 documentation
    // for this API.  I am confused by how it only returns an int,
    // yet the API documentation talks about returning an int, double
    // or text string.
    double d = data.getDouble();
    setDouble(col, d);
}

void Statement::setFloat(const std::string& col, float data)
{
    setDouble(col, static_cast<double>(data));
}

void Statement::setDouble(const std::string& col, double data)
{
    int idx = getBindIndex(col);
    getBindStmt();
    if (idx != 0)
    {
        reset();

        log_debug("sqlite3_bind_double(" << _stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_double(_stmt, idx, data);

        if (ret != SQLITE_OK)
            throw Execerror("sqlite3_bind_double", _stmt, ret);
    }
}

void Statement::setChar(const std::string& col, char data)
{
    int idx = getBindIndex(col);
    getBindStmt();
    if (idx != 0)
    {
        reset();

        log_debug("sqlite3_bind_text(" << _stmt << ", " << idx << ", " << data
          << ", 1, SQLITE_TRANSIENT)");
        int ret = ::sqlite3_bind_text(_stmt, idx, &data, 1, SQLITE_TRANSIENT);

        if (ret != SQLITE_OK)
            throw Execerror("sqlite3_bind_text", _stmt, ret);
    }
}

void Statement::setString(const std::string& col, const std::string& data)
{
    int idx = getBindIndex(col);
    getBindStmt();
    if (idx != 0)
    {
        reset();

        log_debug("sqlite3_bind_text(" << _stmt << ", " << idx << ", " << data
          << ", " << data.size() << ", SQLITE_TRANSIENT)");
        int ret = ::sqlite3_bind_text(_stmt, idx, data.data(), data.size(), SQLITE_TRANSIENT);

        if (ret != SQLITE_OK)
            throw Execerror("sqlite3_bind_text", _stmt, ret);
    }
}

void Statement::setBlob(const std::string& col, const Blob& data)
{
    int idx = getBindIndex(col);
    getBindStmt();
    if (idx != 0)
    {
        reset();

        log_debug("sqlite3_bind_blob(" << _stmt << ", " << idx << ", data, "
            << data.size() << ", SQLITE_TRANSIENT)");
        int ret = ::sqlite3_bind_blob(_stmt, idx, data.data(), data.size(), SQLITE_TRANSIENT);

        if (ret != SQLITE_OK)
            throw Execerror("sqlite3_bind_blob", _stmt, ret);
    }
}

void Statement::setDate(const std::string& col, const Date& data)
{
    setString(col, data.getIso());
}

void Statement::setTime(const std::string& col, const Time& data)
{
    setString(col, data.getIso());
}

void Statement::setDatetime(const std::string& col, const Datetime& data)
{
    setString(col, data.getIso());
}

Statement::size_type Statement::execute()
{
    reset();
    _needReset = true;

    log_debug("sqlite3_step(" << _stmt << ')');
    int ret = sqlite3_step(_stmt);

    if (ret != SQLITE_DONE && ret != SQLITE_ROW)
    {
        log_debug("sqlite3_step failed with return code " << ret);
        throw Execerror("sqlite3_step", _stmt, ret);
    }

    int n = ::sqlite3_changes(::sqlite3_db_handle(_stmt));

    reset();

    return n;
}

Result Statement::select()
{
    reset();
    _needReset = true;

    std::shared_ptr<ResultImpl> result = std::make_shared<ResultImpl>();
    int ret;
    do
    {
        log_debug("sqlite3_step(" << _stmt << ')');
        ret = sqlite3_step(_stmt);

        if (ret == SQLITE_ROW)
        {
            log_debug("sqlite3_column_count(" << _stmt << ')');
            int count = ::sqlite3_column_count(_stmt);
            std::shared_ptr<RowImpl> row = std::make_shared<RowImpl>();
            for (int i = 0; i < count; ++i)
            {
                log_debug("sqlite3_column_bytes(" << _stmt << ", " << i << ')');
                int n = sqlite3_column_bytes(_stmt, i);

                const void* txt = 0;

                if (n > 0)
                {
                    log_debug("sqlite3_column_blob(" << _stmt << ", " << i << ')');
                    txt = sqlite3_column_blob(_stmt, i);
                }

                std::shared_ptr<ValueImpl> v;
                if (txt)
                    v = std::make_shared<ValueImpl>(
                    std::string(static_cast<const char*>(txt), n));

                log_debug("sqlite3_column_name(" << _stmt << ", " << i << ')');
                const char* name = sqlite3_column_name(_stmt, i);
                if (name == 0)
                    throw std::bad_alloc();

                row->add(name, Value(v));
            }

            result->add(Row(row));
        }
        else if (ret != SQLITE_DONE)
        {
            log_debug("sqlite3_step failed with return code " << ret);
            throw Execerror("sqlite3_step", _stmt, ret);
        }

    } while (ret == SQLITE_ROW);

    return Result(result);
}

Row Statement::selectRow()
{
    reset();
    _needReset = true;

    log_debug("sqlite3_step(" << _stmt << ')');
    int ret = sqlite3_step(_stmt);

    if (ret == SQLITE_DONE)
        throw NotFound();
    else if (ret == SQLITE_ROW)
    {
        log_debug("sqlite3_column_count(" << _stmt << ')');
        int count = ::sqlite3_column_count(_stmt);
        std::shared_ptr<RowImpl> row = std::make_shared<RowImpl>();
        for (int i = 0; i < count; ++i)
        {
            log_debug("sqlite3_column_bytes(" << _stmt << ", " << i << ')');
            int n = sqlite3_column_bytes(_stmt, i);

            const void* txt = 0;

            if (n > 0)
            {
                log_debug("sqlite3_column_blob(" << _stmt << ", " << i << ')');
                txt = sqlite3_column_blob(_stmt, i);
            }

            std::shared_ptr<ValueImpl> v;
            if (txt)
                v = std::make_shared<ValueImpl>(
                    std::string(static_cast<const char*>(txt), n));

            log_debug("sqlite3_column_name(" << _stmt << ", " << i << ')');
            const char* name = sqlite3_column_name(_stmt, i);
            if (name == 0)
                throw std::bad_alloc();

            row->add(name, Value(v));
        }

        reset();
        return Row(row);
    }
    else
    {
        reset();
        log_debug("sqlite3_step failed with return code " << ret);
        throw Execerror("sqlite3_step", _stmt, ret);
    }
}

Value Statement::selectValue()
{
    reset();
    _needReset = true;

    log_debug("sqlite3_step(" << _stmt << ')');
    int ret = sqlite3_step(_stmt);

    if (ret == SQLITE_DONE)
    {
        log_debug("sqlite3_step returned SQLITE_DONE => NotFound");
        throw NotFound();
    }
    else if (ret == SQLITE_ROW)
    {
        log_debug("sqlite3_column_count(" << _stmt << ')');
        int count = ::sqlite3_column_count(_stmt);
        if (count == 0)
            throw NotFound();

        log_debug("sqlite3_column_bytes(" << _stmt << ", 0)");
        int n = sqlite3_column_bytes(_stmt, 0);

        const void* txt = 0;

        if (n > 0)
        {
            log_debug("sqlite3_column_blob(" << _stmt << ", 0)");
            txt = sqlite3_column_blob(_stmt, 0);
        }

        std::shared_ptr<ValueImpl> v;
        if (txt)
            v = std::make_shared<ValueImpl>(
                std::string(static_cast<const char*>(txt), n));

        reset();
        return Value(v);
    }
    else
    {
        reset();
        log_debug("sqlite3_step failed with return code " << ret);
        throw Execerror("sqlite3_step", _stmt, ret);
    }
}

std::shared_ptr<ICursor> Statement::createCursor(unsigned fetchsize)
{
    return std::make_shared<Cursor>(getBindStmt());
}

}
}
