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

#include <tntdb/mysql/impl/resultrow.h>
#include <tntdb/mysql/impl/result.h>
#include <tntdb/mysql/impl/rowvalue.h>
#include <tntdb/bits/value.h>
#include <tntdb/error.h>
#include <cxxtools/log.h>

log_define("tntdb.mysql.resultrow")

namespace tntdb
{
namespace mysql
{
ResultRow::ResultRow(std::shared_ptr<Result> resultref, MYSQL_ROW row, unsigned field_count)
    : _resultref(resultref),
      _row(row),
      _field_count(field_count)
{
    log_debug("mysql_fetch_lengths");
    _lengths = ::mysql_fetch_lengths(resultref->getMysqlRes());

    log_debug("mysql_fetch_fields");
    _fields = ::mysql_fetch_fields(resultref->getMysqlRes());
}

ResultRow::ResultRow(MYSQL_RES* res, MYSQL_ROW row, unsigned field_count)
    : _row(row),
      _field_count(field_count)
{
    log_debug("mysql_fetch_lengths");
    _lengths = ::mysql_fetch_lengths(res);

    log_debug("mysql_fetch_fields");
    _fields = ::mysql_fetch_fields(res);
}

unsigned ResultRow::size() const
{
    return _field_count;
}

std::shared_ptr<RowValue> ResultRow::getMysqlValue(std::shared_ptr<ResultRow> result, size_type field_num)
{
    return std::make_shared<RowValue>(result, result->_row, field_num, result->_lengths[field_num]);
}

Value ResultRow::getValueByNumber(size_type field_num) const
{
    return Value(std::make_shared<RowValue>(_row, field_num, _lengths[field_num]));
}

Value ResultRow::getValueByName(const std::string& field_name) const
{
    size_type field_num;
    for (field_num = 0; field_num < size(); ++field_num)
        if (_fields[field_num].name == field_name)
             break;

    if (field_num >= size())
        throw FieldNotFound(field_name);

    return getValueByNumber(field_num);
}

std::string ResultRow::getColumnName(size_type field_num) const
{
    return _fields[field_num].name;
}

}
}
