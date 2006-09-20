/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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
#include <tntdb/smartptr.h>
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
        virtual void setInt(const std::string& col, int data);
        virtual void setUnsigned(const std::string& col, unsigned data);
        virtual void setFloat(const std::string& col, float data);
        virtual void setDouble(const std::string& col, double data);
        virtual void setChar(const std::string& col, char data);
        virtual void setString(const std::string& col, const std::string& data);
        virtual void setDate(const std::string& col, const Date& data);
        virtual void setTime(const std::string& col, const Time& data);
        virtual void setDatetime(const std::string& col, const Datetime& data);

        virtual size_type execute();
        virtual tntdb::Result select();
        virtual tntdb::Row selectRow();
        virtual tntdb::Value selectValue();
        virtual ICursor* createCursor();

        // specific methods of sqlite-driver
        sqlite3_stmt* getStmt() const   { return stmt; }

        void putback(sqlite3_stmt* stmt);
    };
  }
}

#endif // TNTDB_SQLITE_IMPL_SQLSTATEMENT_H

