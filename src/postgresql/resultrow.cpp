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

#include <tntdb/postgresql/impl/resultrow.h>
#include <tntdb/postgresql/impl/result.h>
#include <tntdb/postgresql/impl/resultvalue.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/value.h>
#include <tntdb/error.h>

namespace tntdb
{
namespace postgresql
{
ResultRow::ResultRow(const std::shared_ptr<Result>& resultref, size_type rownumber)
    : _resultref(resultref),
      _result(*resultref),
      _rownumber(rownumber)
{ }

ResultRow::ResultRow(const Result& result, size_type rownumber)
    : _result(result),
      _rownumber(rownumber)
{ }

unsigned ResultRow::size() const
{
    return _result.getFieldCount();
}

Value ResultRow::getValueByNumber(size_type field_num) const
{
    return Value(std::make_shared<ResultValue>(_resultref, _result, _rownumber, field_num));
}

Value ResultRow::getValueByName(const std::string& field_name) const
{
    unsigned fc = _result.getFieldCount();
    unsigned n;
    for (n = 0; n < fc; ++n)
        if (field_name == PQfname(getPGresult(), n))
            break;

    if (n == fc)
        throw FieldNotFound(field_name);

    return getValueByNumber(n);
}

std::string ResultRow::getColumnName(size_type field_num) const
{
    return PQfname(getPGresult(), field_num);
}

PGresult* ResultRow::getPGresult() const
{
    return _result.getPGresult();
}
}
}
