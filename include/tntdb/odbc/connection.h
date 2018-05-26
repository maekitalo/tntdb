/*
 * Copyright (C) 2018 Tommi Maekitalo
 *
 */

#ifndef TNTDB_ODBC_CONNECTION_H
#define TNTDB_ODBC_CONNECTION_H

#include <tntdb/iface/iconnection.h>
#include <sql.h>

namespace tntdb
{
    namespace odbc
    {
        class Connection : public IStmtCacheConnection
        {
            SQLHENV _hEnv;
            SQLHDBC _hDbc;
            SQLHSTMT _hStmt;

        public:
            explicit Connection(const char* conninfo);
            ~Connection();

            void beginTransaction();
            void commitTransaction();
            void rollbackTransaction();

            size_type execute(const std::string& query);
            tntdb::Result select(const std::string& query);
            tntdb::Row selectRow(const std::string& query);
            tntdb::Value selectValue(const std::string& query);
            tntdb::Statement prepare(const std::string& query);
            tntdb::Statement prepareWithLimit(const std::string& query, const std::string& limit, const std::string& offset);
            bool ping();
            long lastInsertId(const std::string& name);
            void lockTable(const std::string& tablename, bool exclusive);

            SQLHENV envHandle() const    { return _hEnv; }
            SQLHDBC handle() const       { return _hDbc; }
        };

    }
}

#endif
