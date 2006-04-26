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

#ifndef TNTDB_IMPL_RESULT_H
#define TNTDB_IMPL_RESULT_H

#include <tntdb/iface/iresult.h>
#include <tntdb/result.h>
#include <vector>

namespace tntdb
{
  class ResultImpl : public IResult
  {
    public:
      typedef std::vector<Row> data_type;

    private:
      data_type data;

    public:
      ResultImpl()
      { }

      explicit ResultImpl(const data_type& data_)
        : data(data_)
      { }

      // methods from IResult
      virtual Row getRow(size_type tup_num) const;
      virtual size_type size() const;
      virtual size_type getFieldCount() const;

      // specific methods
      void add(const Row& row)   { data.push_back(row); }
  };
}

#endif // TNTDB_IMPL_RESULT_H

