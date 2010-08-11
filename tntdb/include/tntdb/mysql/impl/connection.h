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

#ifndef TNTDB_MYSQL_IMPL_CONNECTION_H
#define TNTDB_MYSQL_IMPL_CONNECTION_H

#include <tntdb/iface/iconnection.h>
#include <mysql.h>

namespace tntdb
{
  namespace mysql
  {
    class Connection : public IStmtCacheConnection
    {
        MYSQL mysql;
        bool initialized;
        unsigned transactionActive;

        void open(const char* app, const char* host,
          const char* user, const char* passwd,
          const char* db, unsigned int port,
          const char* unix_socket, unsigned long client_flag);

      public:
        explicit Connection(const char* conn);
        Connection(const char* app, const char* host,
          const char* user, const char* passwd,
          const char* db, unsigned int port = 3306,
          const char* unix_socket = 0, unsigned long client_flag = 0);
        ~Connection();

        MYSQL* getHandle()         { return &mysql; }

        void beginTransaction();
        void commitTransaction();
        void rollbackTransaction();

        size_type execute(const std::string& query);
        tntdb::Result select(const std::string& query);
        tntdb::Row selectRow(const std::string& query);
        tntdb::Value selectValue(const std::string& query);
        tntdb::Statement prepare(const std::string& query);
        bool ping();
        long lastInsertId(const std::string& name);
    };
  }
}

#endif // TNTDB_MYSQL_IMPL_CONNECTION_H

