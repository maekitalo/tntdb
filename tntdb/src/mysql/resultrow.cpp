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

#include <tntdb/mysql/impl/resultrow.h>
#include <tntdb/mysql/impl/rowvalue.h>
#include <tntdb/bits/value.h>
#include <tntdb/error.h>
#include <cxxtools/log.h>

log_define("tntdb.mysql.resultrow")

namespace tntdb
{
  namespace mysql
  {
    ResultRow::ResultRow(const tntdb::Result& result_, MYSQL_RES* res,
      MYSQL_ROW row_)
      : result(result_),
        row(row_)
    {
      log_debug("mysql_fetch_lengths");
      lengths = ::mysql_fetch_lengths(res);

      log_debug("mysql_fetch_fields");
      fields = ::mysql_fetch_fields(res);
    }

    unsigned ResultRow::size() const
    {
      return result.getFieldCount();
    }

    Value ResultRow::getValueByNumber(size_type field_num) const
    {
      return Value(new RowValue(result, row, field_num, lengths[field_num]));
    }

    Value ResultRow::getValueByName(const std::string& field_name) const
    {
      size_type field_num;
      for (field_num = 0; field_num < size(); ++field_num)
        if (fields[field_num].name == field_name)
          break;

      if (field_num >= size())
        throw FieldNotFound(field_name);

      return getValueByNumber(field_num);
    }

  }
}
