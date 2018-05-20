/*
 * Copyright (C) 2018 Tommi Maekitalo
 *
 */

#ifndef TNTDB_ODBC_STATEMENT_H
#define TNTDB_ODBC_STATEMENT_H

#include <tntdb/iface/istatement.h>
#include <tntdb/odbc/handle.h>
#include <sqltypes.h>

namespace tntdb
{
    namespace odbc
    {
        class Connection;

        class Statement : public IStatement
        {
            Connection* _conn;
            Handle _hStmt;

        public:
            Statement(Connection* conn, const std::string& query);
            ~Statement();
            void clear();
            void setNull(const std::string& col);
            void setBool(const std::string& col, bool data);
            void setShort(const std::string& col, short data);
            void setInt(const std::string& col, int data);
            void setLong(const std::string& col, long data);
            void setUnsignedShort(const std::string& col, unsigned short data);
            void setUnsigned(const std::string& col, unsigned data);
            void setUnsignedLong(const std::string& col, unsigned long data);
            void setInt32(const std::string& col, int32_t data);
            void setUnsigned32(const std::string& col, uint32_t data);
            void setInt64(const std::string& col, int64_t data);
            void setUnsigned64(const std::string& col, uint64_t data);
            void setDecimal(const std::string& col, const Decimal& data);
            void setFloat(const std::string& col, float data);
            void setDouble(const std::string& col, double data);
            void setChar(const std::string& col, char data);
            void setString(const std::string& col, const std::string& data);
            void setBlob(const std::string& col, const Blob& data);
            void setDate(const std::string& col, const Date& data);
            void setTime(const std::string& col, const Time& data);
            void setDatetime(const std::string& col, const Datetime& data);

            size_type execute();
            tntdb::Result select();
            tntdb::Row selectRow();
            tntdb::Value selectValue();
            ICursor* createCursor(unsigned fetchsize);
        };
    }
}

#endif

