/* 
   Copyright (C) 2005 Tommi Maekitalo

This file is part of tntdb.

Tntdb is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntdb is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntdb; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
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

        void open(const char* host,
          const char* user, const char* passwd,
          const char* db, unsigned int port,
          const char* unix_socket, unsigned long client_flag);

      public:
        explicit Connection(const char* conn);
        Connection(const char* host,
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

        // specific methods
        my_ulonglong getInsertId();
    };
  }
}

#endif // TNTDB_MYSQL_IMPL_CONNECTION_H

