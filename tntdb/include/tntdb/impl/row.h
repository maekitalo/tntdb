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

