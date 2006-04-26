/* 
   Copyright (C) 2006 Tommi Maekitalo

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
  };
}

#endif // TNTDB_IMPL_POOLCONNECTION_H

