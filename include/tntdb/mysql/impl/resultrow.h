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

#ifndef TNTDB_MYSQL_IMPL_RESULTROW_H
#define TNTDB_MYSQL_IMPL_RESULTROW_H

#include <tntdb/bits/result.h>
#include <tntdb/iface/irow.h>
#include <mysql.h>

namespace tntdb
{
namespace mysql
{
class Result;
class RowValue;

/// Row of a result-set of type Result
class ResultRow : public IRow
{
    std::shared_ptr<Result> _resultref;
    MYSQL_ROW _row;
    unsigned _field_count;
    unsigned long* _lengths;
    MYSQL_FIELD* _fields;

public:
    ResultRow(std::shared_ptr<Result> resultref, MYSQL_ROW row, unsigned field_count);
    ResultRow(MYSQL_RES* res, MYSQL_ROW row, unsigned field_count);

    static std::shared_ptr<RowValue> getMysqlValue(std::shared_ptr<ResultRow> result, size_type field_num);

    unsigned size() const;
    Value getValueByNumber(size_type field_num) const;
    Value getValueByName(const std::string& field_name) const;
    std::string getColumnName(size_type field_num) const;
};
}
}

#endif // TNTDB_MYSQL_IMPL_RESULTROW_H

