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

#ifndef TNTDB_IFACE_IROW_H
#define TNTDB_IFACE_IROW_H

#include <tntdb/refcounted.h>
#include <tntdb/noncopyable.h>

namespace tntdb
{
  class Value;

  /// common interface for row-types
  class IRow : public RefCounted, private NonCopyable
  {
    public:
      typedef unsigned size_type;

      virtual size_type size() const = 0;
      virtual Value getValue(size_type field_num) const = 0;
  };
}

#endif // TNTDB_IFACE_IROW_H

