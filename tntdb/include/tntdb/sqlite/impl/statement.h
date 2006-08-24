/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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
        SmartPtr<Connection, InternalRefCounted> conn;
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

