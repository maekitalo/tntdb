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

#include <tntdb/postgresql/impl/result.h>
#include <tntdb/postgresql/impl/resultrow.h>
#include <tntdb/row.h>
#include <cxxtools/log.h>

log_define("tntdb.postgresql.result")

namespace tntdb
{
  namespace postgresql
  {
    Result::Result(const tntdb::Connection& c, PGresult* r)
      : conn(c),
        result(r)
    {
      log_debug("postgresql-result " << r);
    }

    Result::~Result()
    {
      if (result)
      {
        log_debug("PQclear(" << result << ')');
        ::PQclear(result);
      }
    }

    Row Result::getRow(size_type tup_num) const
    {
      return Row(new ResultRow(const_cast<Result*>(this), tup_num));
    }

    Result::size_type Result::size() const
    {
      log_debug("PQntuples(" << result << ')');
      return ::PQntuples(result);
    }

    Result::size_type Result::getFieldCount() const
    {
      log_debug("PQnfields(" << result << ')');
      return ::PQnfields(result);
    }
  }
}
