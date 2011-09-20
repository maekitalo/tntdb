/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <tntdb/replicate/connection.h>
#include <tntdb/replicate/statement.h>
#include <tntdb/connect.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <tntdb/transaction.h>

namespace tntdb
{
  namespace replicate
  {
    Connection::Connection(const char* conninfo)
    {
      const char* b = conninfo;
      const char* e = conninfo;
      while (*e)
      {
        if (*e == '|')
        {
          connections.push_back(connect(std::string(b, e)));
          b = e+1;
        }
        ++e;
      }

      connections.push_back(connect(std::string(b, e)));
    }

    Connection::~Connection()
    {
      clearStatementCache();
    }

    void Connection::beginTransaction()
    {
      for (Connections::iterator it = connections.begin(); it != connections.end(); ++it)
        it->beginTransaction();
    }

    void Connection::commitTransaction()
    {
      for (Connections::iterator it = connections.begin(); it != connections.end(); ++it)
        it->commitTransaction();
    }

    void Connection::rollbackTransaction()
    {
      for (Connections::iterator it = connections.begin(); it != connections.end(); ++it)
        it->rollbackTransaction();
    }

    Connection::size_type Connection::execute(const std::string& query)
    {
      tntdb::Connection c(this);
      Transaction transaction(c);
      Connections::iterator it = connections.begin();
      size_type ret = it->execute(query);
      for (++it; it != connections.end(); ++it)
        it->execute(query);
      transaction.commit();
      return ret;
    }

    tntdb::Result Connection::select(const std::string& query)
    {
      return connections.begin()->select(query);
    }

    tntdb::Row Connection::selectRow(const std::string& query)
    {
      return connections.begin()->selectRow(query);
    }

    tntdb::Value Connection::selectValue(const std::string& query)
    {
      return connections.begin()->selectValue(query);
    }

    tntdb::Statement Connection::prepare(const std::string& query)
    {
      return tntdb::Statement(new Statement(this, query));
    }

    bool Connection::ping()
    {
      for (Connections::iterator it = connections.begin(); it != connections.end(); ++it)
        if (!it->ping())
          return false;
      return false;
    }

    long Connection::lastInsertId(const std::string& name)
    {
      return connections.begin()->lastInsertId(name);
    }

  }
}
