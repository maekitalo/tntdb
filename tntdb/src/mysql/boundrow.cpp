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

#include <tntdb/mysql/impl/boundrow.h>
#include <tntdb/mysql/impl/boundvalue.h>
#include <tntdb/value.h>
#include <tntdb/error.h>

namespace tntdb
{
  namespace mysql
  {
    BoundRow::size_type BoundRow::size() const
    {
      return BindValues::getSize();
    }

    Value BoundRow::getValueByNumber(size_type field_num) const
    {
      // TODO eliminate creation of BoundValue, by preallocating them.
      // We can maintain a vector<BoundValue> and prevent dynamic deallocation
      // of elements by overriding BoundValue::release
      return Value(new BoundValue(const_cast<IRow*>(static_cast<const IRow*>(this)), getMysqlBind()[field_num]));
    }

    Value BoundRow::getValueByName(const std::string& field_name) const
    {
      size_type field_num;
      for (field_num = 0; field_num < size(); ++field_num)
        if (getName(field_num) == field_name)
          break;

      if (field_num >= size())
        throw FieldNotFound(field_name);

      return getValueByNumber(field_num);
    }
  }
}
