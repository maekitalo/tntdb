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

#include <tntdb/postgresql/impl/resultrow.h>
#include <tntdb/postgresql/impl/result.h>
#include <tntdb/postgresql/impl/resultvalue.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/value.h>
#include <tntdb/error.h>
#include <cxxtools/log.h>

log_define("tntdb.postgresql.resultrow")

namespace tntdb
{
  namespace postgresql
  {
    ResultRow::ResultRow(Result* result_, size_type rownumber_)
      : tntdbResult(result_),
        result(result_),
        rownumber(rownumber_)
    {
    }

    unsigned ResultRow::size() const
    {
      return result->getFieldCount();
    }

    Value ResultRow::getValueByNumber(size_type field_num) const
    {
      return Value(new ResultValue(const_cast<ResultRow*>(this), field_num));
    }

    Value ResultRow::getValueByName(const std::string& field_name) const
    {
      unsigned fc = result->getFieldCount();
      unsigned n;
      for (n = 0; n < fc; ++n)
        if (field_name == PQfname(getPGresult(), n))
          break;

      if (n == fc)
        throw FieldNotFound(field_name);

      return getValueByNumber(n);
    }

    PGresult* ResultRow::getPGresult() const
    {
      return result->getPGresult();
    }
  }
}
