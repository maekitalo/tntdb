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

#ifndef TNTDB_IFACE_ICONNECTION_H
#define TNTDB_IFACE_ICONNECTION_H

#include <cxxtools/refcounted.h>
#include <cxxtools/smartptr.h>
#include <string>
#include <map>

namespace tntdb
{
  class Result;
  class Row;
  class Value;
  class Statement;
  class IStatement;

  class IConnection : public cxxtools::RefCounted
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
      virtual Statement prepareCached(const std::string& query, const std::string& key) = 0;
      virtual void clearStatementCache() = 0;
      virtual bool ping() = 0;
      virtual long lastInsertId(const std::string& name) = 0;
  };

  class IStmtCacheConnection : public IConnection
  {
      typedef std::map<std::string, cxxtools::SmartPtr<IStatement> > stmtCacheType;
      stmtCacheType stmtCache;

    public:
      virtual Statement prepareCached(const std::string& query, const std::string& key);
      virtual void clearStatementCache();
  };
}

#endif // TNTDB_IFACE_ICONNECTION_H

