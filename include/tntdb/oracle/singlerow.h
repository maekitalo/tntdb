/*
 * Copyright (C) 2012 Tommi Maekitalo
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

#ifndef TNTDB_ORACLE_SINGLEROW_H
#define TNTDB_ORACLE_SINGLEROW_H

#include <tntdb/oracle/multirow.h>
#include <tntdb/oracle/singlevalue.h>
#include <tntdb/iface/irow.h>

namespace tntdb
{
namespace oracle
{
class SingleRow : public IRow
{
    std::shared_ptr<MultiRow> _mr;
    unsigned _row;
    typedef std::vector<std::shared_ptr<SingleValue>> Values;
    Values _values;

public:
    SingleRow(std::shared_ptr<MultiRow> mr, unsigned row);

    unsigned row() const  { return _row; }
    void row(unsigned r);

    virtual size_type size() const;
    virtual tntdb::Value getValueByNumber(size_type field_num) const;
    virtual tntdb::Value getValueByName(const std::string& field_name) const;
    virtual std::string getColumnName(size_type field_num) const;
};
}
}

#endif // TNTDB_ORACLE_SINGLEROW_H

