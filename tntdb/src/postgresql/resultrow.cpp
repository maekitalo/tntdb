/* 
 * Copyright (C) 2005 Tommi Maekitalo
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
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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
