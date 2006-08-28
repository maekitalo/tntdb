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

#ifndef TNTDB_MYSQL_IMPL_RESULTROW_H
#define TNTDB_MYSQL_IMPL_RESULTROW_H

#include <tntdb/bits/result.h>
#include <tntdb/iface/irow.h>
#include <mysql.h>

namespace tntdb
{
  namespace mysql
  {
    /// Row of a result-set of type Result
    class ResultRow : public IRow
    {
        Result result;
        MYSQL_ROW row;
        unsigned long* lengths;

      public:
        ResultRow(const tntdb::Result& result_, MYSQL_RES* res, MYSQL_ROW row_);

        unsigned size() const;
        Value getValue(size_type field_num) const;
    };
  }
}

#endif // TNTDB_MYSQL_IMPL_RESULTROW_H

