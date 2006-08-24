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
