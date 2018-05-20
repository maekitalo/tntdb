/*
 * Copyright (C) 2018 Tommi Maekitalo
 *
 */

#include <tntdb/odbc/connection.h>
#include <tntdb/statement.h>
#include <tntdb/result.h>
#include <tntdb/value.h>
#include <tntdb/error.h>

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
{
    long ret;

    log_debug("connect odbc (\"" << conninfo << "\")");
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
        throw tntdb::Error("Unable to allocate an environment handle");
 
	ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
        throw tntdb::Error("Unable to set odbc version");
 
    // Allocate a connection 
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc); 
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
        throw tntdb::Error("Unable to allocate odbc connection handle");

	ret = SQLConnect(hDbc, (SQLCHAR*) conninfo, SQL_NTS, 0, SQL_NTS, 0, SQL_NTS);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
        throw tntdb::Error("Unable to connect to database");

    log_debug("connected succesfully");
}

Connection::~Connection()
{
    long ret;

    ret = SQLDisconnect(hDbc);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
        log_error("failed to disconnect from database");

    ret = SQLFreeHandle(SQL_HANDLE_ENV, hDbc);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
        log_error("failed to free database handle");

    ret = SQLFreeHandle(SQL_HANDLE_DBC, hEnv);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
        log_error("failed to free environment handle");
}

void Connection::beginTransaction()
{
    // TODO
}

void Connection::commitTransaction()
{
    // TODO
}

void Connection::rollbackTransaction()
{
    // TODO
}

Connection::size_type Connection::execute(const std::string& query)
{
    // TODO
    throw Error("execute not implemented yet");
}

tntdb::Result Connection::select(const std::string& query)
{
    // TODO
    throw Error("select not implemented yet");
}

tntdb::Row Connection::selectRow(const std::string& query)
{
    // TODO
    throw Error("selectRow not implemented yet");
}

tntdb::Value Connection::selectValue(const std::string& query)
{
    // TODO
    throw Error("selectValue not implemented yet");
}

tntdb::Statement Connection::prepare(const std::string& query)
{
    // TODO
    throw Error("prepare not implemented yet");
}

tntdb::Statement Connection::prepareWithLimit(const std::string& query, const std::string& limit, const std::string& offset)
{
    // TODO
    throw Error("prepareWithLimit not implemented yet");
}

bool Connection::ping()
{
    return true;
}

long Connection::lastInsertId(const std::string& name)
{
    return 0l;
}

void Connection::lockTable(const std::string& tablename, bool exclusive)
{
}

bool ping();
long lastInsertId(const std::string& name);
void lockTable(const std::string& tablename, bool exclusive);
}

}
