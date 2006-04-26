/* 
   Copyright (C) 2005 Tommi Maekitalo

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

#include <tntdb/postgresql/impl/resultrow.h>
#include <tntdb/postgresql/impl/result.h>
#include <tntdb/postgresql/impl/resultvalue.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/value.h>
#include <cxxtools/log.h>

log_define("tntdb.postgresql.resultrow");

namespace tntdb
{
  namespace postgresql
  {
    ResultRow::ResultRow(Result* result_, size_type rownumber_)
      : tntdbResult(result_),
        result(result_),
        rownumber(rownumber_)
    {
      if (result)
        result->addRef();
    }

    ResultRow::~ResultRow()
    {
      if (result)
        result->release();
    }

    unsigned ResultRow::size() const
    {
      return result->getFieldCount();
    }

    Value ResultRow::getValue(size_type field_num) const
    {
      return Value(new ResultValue(const_cast<ResultRow*>(this), field_num));
    }

    PGresult* ResultRow::getPGresult() const
    {
      return result->getPGresult();
    }
  }
}
