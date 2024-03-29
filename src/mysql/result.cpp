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
Result::Result(MYSQL* m, MYSQL_RES* r)
  : mysql(m),
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

std::shared_ptr<ResultRow> Result::getMysqlRow(size_type tup_num) const
{
    log_debug("mysql_data_seek(" << tup_num << ')');
    ::mysql_data_seek(result, tup_num);

    log_debug("mysql_fetch_row");
    MYSQL_ROW row = ::mysql_fetch_row(result);
    if (row == 0)
      throw MysqlError("mysql_fetch_row", mysql);

    return std::make_shared<ResultRow>(result, row, field_count);
}

Row Result::getRow(size_type tup_num) const
{
    return Row(getMysqlRow(tup_num));
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
