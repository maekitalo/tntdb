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

