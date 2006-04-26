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

#include <tntdb/mysql/impl/result.h>
#include <tntdb/mysql/impl/resultrow.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/connection.h>
#include <tntdb/mysql/error.h>
#include <cxxtools/log.h>

log_define("tntdb.mysql.result")

namespace tntdb
{
  namespace mysql
  {
    Result::Result(const tntdb::Connection& c, MYSQL* m, MYSQL_RES* r)
      : conn(c),
        mysql(m),
        result(r)
    {
      log_debug("mysql-result " << r);

      log_debug("mysql_field_count");
      field_count = ::mysql_field_count(m);
    }

    Result::~Result()
    {
      if (result)
      {
        log_debug("mysql_free_result(" << result << ')');
        ::mysql_free_result(result);
      }
    }

    Row Result::getRow(size_type tup_num) const
    {
      log_debug("mysql_data_seek(" << tup_num << ')');
      ::mysql_data_seek(result, tup_num);

      log_debug("mysql_fetch_row");
      MYSQL_ROW row = ::mysql_fetch_row(result);
      if (row == 0)
        throw MysqlError("mysql_fetch_row", mysql);

      const IResult* resc = this;
      IResult* res = const_cast<IResult*>(resc);
      return Row(new ResultRow(tntdb::Result(res), result, row));
    }

    Result::size_type Result::size() const
    {
      log_debug("mysql_num_rows");
      return ::mysql_num_rows(result);
    }

    Result::size_type Result::getFieldCount() const
    {
      return field_count;
    }
  }
}
