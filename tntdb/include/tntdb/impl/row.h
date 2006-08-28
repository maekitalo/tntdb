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

#ifndef TNTDB_IMPL_ROW_H
#define TNTDB_IMPL_ROW_H

#include <tntdb/iface/irow.h>
#include <tntdb/value.h>
#include <vector>

namespace tntdb
{
  class RowImpl : public IRow
  {
    public:
      typedef std::vector<Value> data_type;

    private:
      data_type data;

    public:
      RowImpl()
        { }

      explicit RowImpl(const data_type& data_)
        : data(data_)
        { }

      // methods from IResult
      virtual size_type size() const;
      virtual Value getValue(size_type field_num) const;

      // specific methods
      void add(const Value& value)   { data.push_back(value); }
  };
}

#endif // TNTDB_IMPL_ROW_H

