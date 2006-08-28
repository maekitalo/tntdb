/* 
 * Copyright (C) 2005 Tommi Maekitalo
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

#ifndef TNTDB_IFACE_ICONNECTION_H
#define TNTDB_IFACE_ICONNECTION_H

#include <tntdb/refcounted.h>
#include <tntdb/smartptr.h>
#include <tntdb/noncopyable.h>
#include <string>
#include <map>

namespace tntdb
{
  class Result;
  class Row;
  class Value;
  class Statement;
  class IStatement;

  class IConnection : public RefCounted, private NonCopyable
  {
    public:
      typedef unsigned size_type;

      virtual void beginTransaction() = 0;
      virtual void commitTransaction() = 0;
      virtual void rollbackTransaction() = 0;

      virtual size_type execute(const std::string& query) = 0;
      virtual Result select(const std::string& query) = 0;
      virtual Row selectRow(const std::string& query) = 0;
      virtual Value selectValue(const std::string& query) = 0;
      virtual Statement prepare(const std::string& query) = 0;
      virtual Statement prepareCached(const std::string& query) = 0;
      virtual void clearStatementCache() = 0;
  };

  class IStmtCacheConnection : public IConnection
  {
      typedef std::map<std::string, SmartPtr<IStatement> > stmtCacheType;
      stmtCacheType stmtCache;

    public:
      virtual Statement prepareCached(const std::string& query);
      virtual void clearStatementCache();
  };
}

#endif // TNTDB_IFACE_ICONNECTION_H

