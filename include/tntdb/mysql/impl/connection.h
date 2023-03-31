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
/**

 This namespace contains the implementation of the Mysql driver for tntdb.

 The driver makes it possible to access a Mysql database using tntdb.

 To get a connection to a Mysql database, the dburl to the tntdb::connect function
 must start with "mysql:".

 The remaining is a list of name value pairs separated by semicolon. The names and
 values are separated by '='. The value may be quoted by single or double quotes.
 Quote characters inside quoted values may be escaped using a backspace. If the value
 contains itself a backspace character, it must be doubled. Indeed each character
 prefixed with backspace is kept as is into the value.

 A typical connection with a Mysql driver looks like that:

 @code
   tntdb::Connection conn = tntdb::connect("mysql:db=DS2;user=web;passwd='foo\\'bar");
 @endcode

 Here the username is "web" and the password is "foo'bar". Note that the backslash
 itself must be doubled in C++ code since the compiler processes the backspace first.

 */

namespace mysql
{
/// Implements a connection to a Mysql database.
class Connection : public IStmtCacheConnection
{
    MYSQL mysql;
    bool initialized;
    unsigned transactionActive;
    std::string lockTablesQuery;

    void open(const char* app, const char* host,
      const char* user, const char* passwd,
      const char* db, unsigned int port,
      const char* unix_socket, unsigned long client_flag);

public:
    Connection(const std::string& conn, const std::string& username, const std::string& password);
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
    tntdb::Statement prepareWithLimit(const std::string& query, const std::string& limit, const std::string& offset);
    bool ping();
    long lastInsertId(const std::string& name);
    void lockTable(const std::string& tablename, bool exclusive);
};
}
}

#endif // TNTDB_MYSQL_IMPL_CONNECTION_H

