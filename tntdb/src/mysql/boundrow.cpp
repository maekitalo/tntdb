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
