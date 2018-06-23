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

#include <tntdb/odbc/connection.h>
#include <tntdb/odbc/statement.h>
#include <tntdb/odbc/handle.h>
#include <tntdb/odbc/error.h>

#include <tntdb/statement.h>
#include <tntdb/result.h>
#include <tntdb/value.h>

#include <cxxtools/log.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

log_define("tntdb.odbc.connection")

namespace tntdb
{
namespace odbc
{

Connection::Connection(const char* conninfo)
    : _hEnv(0),
      _hDbc(0),
      _hStmt(0)
{
    SQLRETURN retval;

    log_debug("connect odbc (\"" << conninfo << "\")");
    retval = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_hEnv);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw tntdb::Error("Unable to allocate an odbc environment handle");
 
	retval = SQLSetEnvAttr(_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("Unable to set odbc version", retval, SQL_HANDLE_ENV, _hEnv);
 
    // Allocate a connection 
    retval = SQLAllocHandle(SQL_HANDLE_DBC, _hEnv, &_hDbc); 
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw tntdb::Error("Unable to allocate odbc connection handle");

	retval = SQLConnect(_hDbc, (SQLCHAR*) conninfo, SQL_NTS, 0, SQL_NTS, 0, SQL_NTS);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("Unable to connect to database", retval, SQL_HANDLE_ENV, _hEnv);

    log_debug("connected succesfully");
}

Connection::~Connection()
{
    SQLRETURN retval;

    retval = SQLDisconnect(_hDbc);
    log_error_if(retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO,
        "failed to disconnect from database");

    if (_hStmt)
    {
        retval = SQLFreeHandle(SQL_HANDLE_STMT, _hStmt);
        log_error_if(retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO,
            "failed to free statement handle");
    }

    retval = SQLFreeHandle(SQL_HANDLE_DBC, _hDbc);
    log_error_if(retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO,
        "failed to free database handle");

    retval = SQLFreeHandle(SQL_HANDLE_ENV, _hEnv);
    log_error_if(retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO,
        "failed to free environment handle");

}

void Connection::beginTransaction()
{
    SQLRETURN retval;

    retval = SQLSetConnectAttr(_hDbc,
        SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)0, 0);

    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("Failed to start transaction", retval, SQL_HANDLE_DBC, _hDbc);
}

void Connection::commitTransaction()
{
    SQLRETURN retval;

    retval = SQLEndTran(SQL_HANDLE_DBC, _hDbc, SQL_COMMIT);

    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("Failed to commit transaction", retval, SQL_HANDLE_DBC, _hDbc);

    retval = SQLSetConnectAttr(_hDbc,
        SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)1, 0);

    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("Failed to disable transaction", retval, SQL_HANDLE_DBC, _hDbc);
}

void Connection::rollbackTransaction()
{
    SQLRETURN retval;

    retval = SQLEndTran(SQL_HANDLE_DBC, _hDbc, SQL_ROLLBACK);

    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("Failed to rollback transaction", retval, SQL_HANDLE_DBC, _hDbc);

    retval = SQLSetConnectAttr(_hDbc,
        SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)1, 0);

    if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("Failed to disable transaction", retval, SQL_HANDLE_DBC, _hDbc);
}

Connection::size_type Connection::execute(const std::string& query)
{
    SQLRETURN retval;

    //Handle hStmt(SQL_HANDLE_STMT, SQL_HANDLE_DBC, _hDbc);
    if (!_hStmt)
    {
        log_debug("SQLAllocHandle(SQL_HANDLE_STMT)");
        retval = SQLAllocHandle(SQL_HANDLE_STMT, _hDbc, &_hStmt);

        if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
            throw Error("Unable to allocate statement handle", retval, SQL_HANDLE_DBC, _hDbc);
    }

    log_debug("SQLExecDirect <" << query << '>');
    retval = SQLExecDirect(_hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("Unable to execute sql query <" + query + '>', retval, SQL_HANDLE_STMT, _hStmt);

    SQLLEN rowCount;
    retval = SQLRowCount(_hStmt, &rowCount);
	if (retval != SQL_SUCCESS && retval != SQL_SUCCESS_WITH_INFO)
        throw Error("SQLRowCount failed", retval, SQL_HANDLE_STMT, _hStmt);

    log_debug(rowCount << " rows affected");
    return static_cast<Connection::size_type>(rowCount);
}

tntdb::Result Connection::select(const std::string& query)
{
    return prepare(query).select();
}

tntdb::Row Connection::selectRow(const std::string& query)
{
    return prepare(query).selectRow();
}

tntdb::Value Connection::selectValue(const std::string& query)
{
    return prepare(query).selectValue();
}

tntdb::Statement Connection::prepare(const std::string& query)
{
    return tntdb::Statement(new Statement(this, query));
}

tntdb::Statement Connection::prepareWithLimit(const std::string& query, const std::string& limit, const std::string& offset)
{
    // TODO
    throw std::runtime_error("tntdb::odbc::Connection::prepareWithLimit not implemented yet");
}

bool Connection::ping()
{
    // TODO
    return true;
}

long Connection::lastInsertId(const std::string& name)
{
    // TODO
    return 0l;
}

void Connection::lockTable(const std::string& tablename, bool exclusive)
{
}

}
}
