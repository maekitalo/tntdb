/* 
 * Copyright (C) 2006 Tommi Maekitalo
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

#ifndef TNTDB_IMPL_POOLCONNECTION_H
#define TNTDB_IMPL_POOLCONNECTION_H

#include <tntdb/connectionpool.h>
#include <tntdb/iface/iconnection.h>

namespace tntdb
{
  class PoolConnection : public IConnection
  {
      ConnectionPool::PoolObjectType connection;
      bool inTransaction;
      bool drop;

    public:
      PoolConnection(ConnectionPool::PoolObjectType connection);
      ~PoolConnection();

      virtual void beginTransaction();
      virtual void commitTransaction();
      virtual void rollbackTransaction();

      virtual size_type execute(const std::string& query);
      virtual Result select(const std::string& query);
      virtual Row selectRow(const std::string& query);
      virtual Value selectValue(const std::string& query);
      virtual Statement prepare(const std::string& query);
      virtual Statement prepareCached(const std::string& query);
      virtual void clearStatementCache();
      virtual bool ping();
  };
}

#endif // TNTDB_IMPL_POOLCONNECTION_H

