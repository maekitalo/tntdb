/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#include <tntdb/oracle/statement.h>
#include <tntdb/oracle/connection.h>
#include <tntdb/oracle/result.h>
#include <tntdb/oracle/row.h>
#include <tntdb/oracle/value.h>
#include <tntdb/oracle/cursor.h>
#include <tntdb/bits/value.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/result.h>
#include <tntdb/error.h>
#include <cxxtools/log.h>

log_define("tntdb.oracle.statement")

namespace tntdb
{
namespace oracle
{
std::shared_ptr<Statement::Bind> Statement::getBindPtr(const std::string& col)
{
    BindMapType::iterator it = bindMap.find(col);
    if (it == bindMap.end())
        it = bindMap.emplace(col, std::make_shared<Bind>()).first;
    return it->second;
}

Statement::Statement(Connection& conn, const std::string& query)
  : _conn(conn),
    _query(query),
    _stmtp(0)
{
    getHandle();
}

Statement::~Statement()
{
    if (_stmtp)
    {
        log_debug("release statement handle " << _stmtp);
        sword ret = OCIHandleFree(_stmtp, OCI_HTYPE_STMT);
        if (ret != OCI_SUCCESS)
            log_error("OCIHandleFree(" << _stmtp << " OCI_HTYPE_STMT) failed");
    }
}

// methods of IStatement

void Statement::clear()
{
    for (BindMapType::iterator it = bindMap.begin(); it != bindMap.end(); ++it)
    {
        it->second->setNull();

        log_debug("OCIBindByName, clear(\"" << it->first << "\")");
        sword ret = OCIBindByName(getHandle(), &it->second->ptr, _conn.getErrorHandle(),
            0, 0, 0, 0,
            SQLT_INT, &it->second->indicator, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");
    }
}

void Statement::setNull(const std::string& col)
{
    log_trace("OCIStmt(" << getHandle() << ").setNull(\"" << col << "\")");

    Bind &b = getBind(col);
    b.setNull();

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_CHR || b.boundLength != 0)
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setNull(\"" << col << "\")");
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
            0, 0,
            SQLT_CHR, &b.indicator, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_CHR;
    }
}

void Statement::setBool(const std::string& col, bool data)
{
    log_trace("OCIStmt(" << getHandle() << ").setBool(\"" << col << "\", " << data << ')');
    Bind &b = getBind(col);
    b.setData(reinterpret_cast<char*>(&data), sizeof(bool));

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_INT || b.boundLength != sizeof(bool))
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setBool(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.data[0], sizeof(bool),
            SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_INT;
        b.boundLength = sizeof(bool);
    }
}

void Statement::setShort(const std::string& col, short data)
{
    log_trace("OCIStmt(" << getHandle() << ").setShort(\"" << col << "\", " << data << ')');

    Bind &b = getBind(col);
    b.setData(reinterpret_cast<char*>(&data), sizeof(short));

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_INT || b.boundLength != sizeof(short))
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setInt(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.data[0], sizeof(short),
            SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_INT;
        b.boundLength = sizeof(short);
    }
}

void Statement::setInt(const std::string& col, int data)
{
    log_trace("OCIStmt(" << getHandle() << ").setInt(\"" << col << "\", " << data << ')');

    Bind &b = getBind(col);
    b.setData(reinterpret_cast<char*>(&data), sizeof(int));

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_INT || b.boundLength != sizeof(int))
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setInt(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.data[0], sizeof(int),
            SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_INT;
        b.boundLength = sizeof(int);
    }
}

void Statement::setLong(const std::string& col, long data)
{
    log_trace("OCIStmt(" << getHandle() << ").setLong(\"" << col << "\", " << data << ')');

    Bind &b = getBind(col);
    b.number.setLong(data, _conn.getErrorHandle());

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_VNU)
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setLong(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
            b.number.getHandle(), OCI_NUMBER_SIZE, SQLT_VNU,
            0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_VNU;
    }
}

void Statement::setUnsignedShort(const std::string& col, unsigned short data)
{
    log_trace("OCIStmt(" << getHandle() << ").setUnsignedShort(\"" << col << "\", " << data << ')');

    Bind &b = getBind(col);
    b.setData(reinterpret_cast<char*>(&data), sizeof(unsigned short));

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_UIN || b.boundLength != sizeof(unsigned short))
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setUnsigned(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.data[0], sizeof(unsigned short),
            SQLT_UIN, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_UIN;
        b.boundLength = sizeof(unsigned short);
    }
}

void Statement::setUnsigned(const std::string& col, unsigned data)
{
    log_trace("OCIStmt(" << getHandle() << ").setUnsigned(\"" << col << "\", " << data << ')');

    Bind &b = getBind(col);
    b.setData(reinterpret_cast<char*>(&data), sizeof(unsigned));

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_UIN || b.boundLength != sizeof(unsigned))
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setUnsigned(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.data[0], sizeof(unsigned),
            SQLT_UIN, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_UIN;
        b.boundLength = sizeof(unsigned);
    }
}

void Statement::setUnsignedLong(const std::string& col, unsigned long data)
{
    log_trace("OCIStmt(" << getHandle() << ").setUnsignedLong(\"" << col << "\", " << data << ')');

    Bind &b = getBind(col);
    b.number.setUnsignedLong(data, _conn.getErrorHandle());

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_VNU)
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setUnsignedLong(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
            b.number.getHandle(), OCI_NUMBER_SIZE, SQLT_VNU,
            0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_VNU;
    }
}

void Statement::setInt32(const std::string& col, int32_t data)
{
    log_trace("OCIStmt(" << getHandle() << ").setInt32(\"" << col << "\", " << data << ')');
    return setInt(col, data);
}

void Statement::setUnsigned32(const std::string& col, uint32_t data)
{
    log_trace("OCIStmt(" << getHandle() << ").setUnsigned32(\"" << col << "\", " << data << ')');
    return setUnsigned(col, data);
}

void Statement::setInt64(const std::string& col, int64_t data)
{
    log_trace("OCIStmt(" << getHandle() << ").setInt64(\"" << col << "\", " << data << ')');

    Bind &b = getBind(col);
    b.number.setInt64(data, _conn.getErrorHandle());

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_VNU)
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setUnsignedLong(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
            b.number.getHandle(), OCI_NUMBER_SIZE, SQLT_VNU,
            0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_VNU;
    }
}


void Statement::setUnsigned64(const std::string& col, uint64_t data)
{
    log_trace("OCIStmt(" << getHandle() << ").setUnsigned64(\"" << col << "\", " << data << ')');

    Bind &b = getBind(col);
    b.number.setUnsigned64(data, _conn.getErrorHandle());

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_VNU)
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setUnsignedLong(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          (void *)b.number.getHandle(), OCI_NUMBER_SIZE, SQLT_VNU,
            0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_VNU;
    }
}

void Statement::setDecimal(const std::string& col, const Decimal& decimal)
{
    log_trace("OCIStmt(" << getHandle() << ").setDecimal(\"" << col << "\", " << decimal << ')');

    Bind &b = getBind(col);
    b.number.setDecimal(decimal, _conn.getErrorHandle());

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_VNU)
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setDecimal(\"" << col << "\", " << decimal << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
            b.number.getHandle(), OCI_NUMBER_SIZE,
            SQLT_VNU, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_VNU;
    }
}

void Statement::setFloat(const std::string& col, float data)
{
    log_trace("OCIStmt(" << getHandle() << ").setFloat(\"" << col << "\", " << data << ')');

    Bind &b = getBind(col);
    b.setData(reinterpret_cast<char*>(&data), sizeof(float));

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_FLT || b.boundLength != sizeof(float))
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setFloat(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.data[0], sizeof(float),
            SQLT_FLT, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_FLT;
        b.boundLength = sizeof(float);
    }
}

void Statement::setDouble(const std::string& col, double data)
{
    log_trace("OCIStmt(" << getHandle() << ").setDouble(\"" << col << "\", " << data << ')');

    Bind &b = getBind(col);
    b.setData(reinterpret_cast<char*>(&data), sizeof(double));

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_FLT || b.boundLength != sizeof(double))
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setDouble(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.data[0], sizeof(double),
            SQLT_FLT, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_FLT;
        b.boundLength = sizeof(double);
    }
}

void Statement::setChar(const std::string& col, char data)
{
    log_trace("OCIStmt(" << getHandle() << ").setChar(\"" << col << "\", '" << data << "')");

    Bind &b = getBind(col);
    b.setData(&data, 1);

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_AFC || b.boundLength != 1)
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setChar(\"" << col << "\", " << data << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.data[0], 1,
            SQLT_AFC, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_AFC;
        b.boundLength = 1;
    }
}

void Statement::setString(const std::string& col, const std::string& data)
{
    log_trace("OCIStmt(" << getHandle() << ").setString(\"" << col << "\", \"" << data << "\")");

    Bind &b = getBind(col);
    b.setData(data);

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_AFC || b.boundLength != data.size())
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setString(\"" << col << "\", \"" << data << "\")");
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.data[0], data.size(),
            SQLT_AFC, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_AFC;
        b.boundLength = data.size();
    }
}

void Statement::setBlob(const std::string& col, const tntdb::Blob& data)
{
    log_trace("OCIStmt(" << getHandle() << ").setBlob(\"" << col << "\", data(" << data.size() << "))");

    Bind &b = getBind(col);
    b.setData(data.data(), data.size());

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_BIN || b.boundLength != data.size())
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setBlob(\"" << col << "\", data{" << data.size() << "})");
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.data[0], data.size(),
            SQLT_BIN, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_BIN;
        b.boundLength = data.size();
    }
}

void Statement::setDate(const std::string& col, const Date& data)
{
    log_trace("OCIStmt(" << getHandle() << ").setDate(\"" << col << "\", \"" << data.getIso() << "\")");

    Bind &b = getBind(col);
    b.datetime.assign(&_conn, data);

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_TIMESTAMP)
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setDate(\"" << col << "\", " << data.getIso() << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.datetime.getHandle(), sizeof(OCIDateTime**),
            SQLT_TIMESTAMP, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_TIMESTAMP;
    }
}

void Statement::setTime(const std::string& col, const Time& data)
{
    log_trace("OCIStmt(" << getHandle() << ").setTime(\"" << col << "\", \"" << data.getIso() << "\")");

    Bind &b = getBind(col);
    b.datetime.assign(&_conn, data);

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_TIMESTAMP)
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setTime(\"" << col << "\", " << data.getIso() << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.datetime.getHandle(), sizeof(OCIDateTime**),
            SQLT_TIMESTAMP, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_TIMESTAMP;
    }
}

void Statement::setDatetime(const std::string& col, const tntdb::Datetime& data)
{
    log_trace("OCIStmt(" << getHandle() << ").setDatetime(\"" << col << "\", \"" << data.getIso() << "\")");

    Bind &b = getBind(col);
    b.datetime.assign(&_conn, data);

    if (b.boundPtr != &b.data[0] || b.boundType != SQLT_TIMESTAMP)
    {
        b.boundPtr = 0;
        b.boundType = 0;
        b.boundLength = 0;

        log_debug("OCIBindByName, setDatetime(\"" << col << "\", " << data.getIso() << ')');
        sword ret = OCIBindByName(getHandle(), &b.ptr, _conn.getErrorHandle(),
            reinterpret_cast<const text*>(col.data()), col.size(),
          &b.datetime.getHandle(), sizeof(OCIDateTime**),
            SQLT_TIMESTAMP, 0, 0, 0, 0, 0, OCI_DEFAULT);

        _conn.checkError(ret, "OCIBindByName");

        b.boundPtr = &b.data[0];
        b.boundType = SQLT_TIMESTAMP;
    }
}

Statement::size_type Statement::execute()
{
    log_debug("execute statement " << getHandle() << " mode=" << (_conn.isTransactionActive() ? "OCI_DEFAULT" : "OCI_COMMIT_ON_SUCCESS"));
    sword ret = OCIStmtExecute(_conn.getSvcCtxHandle(), getHandle(), _conn.getErrorHandle(),
        1, 0, 0, 0, _conn.isTransactionActive() ? OCI_DEFAULT : OCI_COMMIT_ON_SUCCESS);
    _conn.checkError(ret, "OCIStmtExecute");

    ub4 rowcnt = 0;
    ret = OCIAttrGet(getHandle(), OCI_HTYPE_STMT, &rowcnt, 0, OCI_ATTR_ROW_COUNT,
        _conn.getErrorHandle());
    _conn.checkError(ret, "OCIAttrGet");

    return rowcnt;
}

tntdb::Result Statement::select()
{
    return tntdb::Result(std::make_shared<Result>(*this, 64));
}

tntdb::Row Statement::selectRow()
{
    sword ret = OCIStmtExecute(_conn.getSvcCtxHandle(),
        getHandle(), _conn.getErrorHandle(), 0, 0, 0, 0, OCI_DEFAULT);
    _conn.checkError(ret, "OCIStmtExecute");

    auto row = std::make_shared<Row>(*this);
    ret = OCIStmtFetch(getHandle(), _conn.getErrorHandle(), 1, OCI_FETCH_NEXT,
        OCI_DEFAULT);
    _conn.checkError(ret, "OCIStmtFetch");

    return tntdb::Row(row);
}

tntdb::Value Statement::selectValue()
{
    sword ret = OCIStmtExecute(_conn.getSvcCtxHandle(),
        getHandle(), _conn.getErrorHandle(), 0, 0, 0, 0, OCI_DEFAULT);
    _conn.checkError(ret, "OCIStmtExecute");

    auto value = std::make_shared<Value>(*this, 0);
    ret = OCIStmtFetch(getHandle(), _conn.getErrorHandle(), 1, OCI_FETCH_NEXT,
        OCI_DEFAULT);
    _conn.checkError(ret, "OCIStmtFetch");

    return tntdb::Value(value);
}

std::shared_ptr<ICursor> Statement::createCursor(unsigned fetchsize)
{
    return std::make_shared<Cursor>(*this, fetchsize);
}

OCIStmt* Statement::getHandle()
{
    if (_stmtp == 0)
    {
        log_debug("prepare statement \"" << _query << '"');

        sword ret = OCIHandleAlloc(_conn.getEnvHandle(), (void**)&_stmtp, OCI_HTYPE_STMT, 0, 0);
        _conn.checkError(ret, "OCIHandleAlloc(OCI_HTYPE_STMT)");

        ret = OCIStmtPrepare(_stmtp, _conn.getErrorHandle(),
            reinterpret_cast<const unsigned char*>(_query.data()),
            _query.size(), OCI_NTV_SYNTAX, OCI_DEFAULT);
        _conn.checkError(ret, "OCIStmtPrepare");

        log_debug("statement handle " << _stmtp);
    }

    return _stmtp;
}
}
}
