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

#ifndef TNTDB_REPLICATE_IMPL_CONNECTION_H
#define TNTDB_REPLICATE_IMPL_CONNECTION_H

#include <tntdb/iface/iconnection.h>
#include <tntdb/connection.h>
#include <vector>

namespace tntdb
{
  namespace replicate
  {
    class Statement;

    class Connection : public IStmtCacheConnection
    {
        friend class Statement;

        typedef std::vector<tntdb::Connection> Connections;
        Connections connections;

      public:
        explicit Connection(const char* conninfo);
        ~Connection();

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

#endif // TNTDB_REPLICATE_IMPL_CONNECTION_H
