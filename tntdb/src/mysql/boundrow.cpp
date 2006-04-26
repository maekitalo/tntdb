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

#include <tntdb/mysql/impl/boundrow.h>
#include <tntdb/mysql/impl/boundvalue.h>
#include <tntdb/value.h>
#include <cxxtools/log.h>

log_define("tntdb.mysql.boundrow")

namespace tntdb
{
  namespace mysql
  {
    BoundRow::size_type BoundRow::size() const
    {
      return BindValues::getSize();
    }

    Value BoundRow::getValue(size_type field_num) const
    {
      // TODO eliminate creation of BoundValue, by preallocating them.
      // We can maintain a vector<BoundValue> and prevent dynamic deallocation
      // of elements by overriding BoundValue::release
      return Value(new BoundValue(const_cast<IRow*>(static_cast<const IRow*>(this)), getMysqlBind()[field_num]));
    }
  }
}
