/*
 * Copyright (C) 2011 Tommi Maekitalo
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
 */

#include <tntdb/replicate/connection.h>
#include <tntdb/replicate/statement.h>
#include <tntdb/connect.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>

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
      Connections::iterator it = connections.begin();
      size_type ret = it->execute(query);
      for (++it; it != connections.end(); ++it)
        it->execute(query);
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
    }

    long Connection::lastInsertId(const std::string& name)
    {
      return connections.begin()->lastInsertId(name);
    }

  }
}
