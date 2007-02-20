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

#ifndef TNTDB_IFACE_IRESULT_H
#define TNTDB_IFACE_IRESULT_H

#include <tntdb/refcounted.h>

namespace tntdb
{
  class Row;

  /// common inteface for resultset
  class IResult : public RefCounted
  {
    public:
      typedef unsigned size_type;
      typedef Row value_type;

      virtual Row getRow(size_type tup_num) const = 0;
      virtual size_type size() const = 0;
      virtual size_type getFieldCount() const = 0;
  };
}

#endif // TNTDB_IFACE_IRESULT_H

