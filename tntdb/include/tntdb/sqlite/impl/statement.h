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

#ifndef TNTDB_SQLITE_IMPL_SQLSTATEMENT_H
#define TNTDB_SQLITE_IMPL_SQLSTATEMENT_H

#include <tntdb/iface/istatement.h>
#include <sqlite3.h>

namespace tntdb
{
  namespace sqlite
  {
    class Connection;

    class Statement : public IStatement
    {
        sqlite3_stmt* stmt;
        sqlite3_stmt* stmtInUse;
        Connection* conn;
        const std::string query;

        sqlite3_stmt* getBindStmt();
        int getBindIndex(const std::string& col);

        bool needReset;
        void reset();

      public:
        Statement(Connection* conn, const std::string& query);
        ~Statement();

        // methods of IStatement

        virtual void clear();
        virtual void setNull(const std::string& col);
        virtual void setBool(const std::string& col, bool data);
        virtual void setShort(const std::string& col, short data);
        virtual void setInt(const std::string& col, int data);
        virtual void setLong(const std::string& col, long data);
        virtual void setUnsignedShort(const std::string& col, unsigned short data);
        virtual void setUnsigned(const std::string& col, unsigned data);
        virtual void setUnsignedLong(const std::string& col, unsigned long data);
        virtual void setInt32(const std::string& col, int32_t data);
        virtual void setUnsigned32(const std::string& col, uint32_t data);
        virtual void setInt64(const std::string& col, int64_t data);
        virtual void setUnsigned64(const std::string& col, uint64_t data);
        virtual void setDecimal(const std::string& col, const Decimal& data);
        virtual void setFloat(const std::string& col, float data);
        virtual void setDouble(const std::string& col, double data);
        virtual void setChar(const std::string& col, char data);
        virtual void setString(const std::string& col, const std::string& data);
        virtual void setBlob(const std::string& col, const Blob& data);
        virtual void setDate(const std::string& col, const Date& data);
        virtual void setTime(const std::string& col, const Time& data);
        virtual void setDatetime(const std::string& col, const Datetime& data);

        virtual size_type execute();
        virtual tntdb::Result select();
        virtual tntdb::Row selectRow();
        virtual tntdb::Value selectValue();
        virtual ICursor* createCursor(unsigned fetchsize);

        // specific methods of sqlite-driver
        sqlite3_stmt* getStmt() const   { return stmt; }

        void putback(sqlite3_stmt* stmt);
    };
  }
}

#endif // TNTDB_SQLITE_IMPL_SQLSTATEMENT_H

