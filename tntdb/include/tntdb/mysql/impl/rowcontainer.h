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

#ifndef TNTDB_MYSQL_IMPL_ROWCONTAINER_H
#define TNTDB_MYSQL_IMPL_ROWCONTAINER_H

#include <tntdb/iface/iresult.h>
#include <tntdb/iface/irow.h>
#include <vector>
#include <tntdb/smartptr.h>

namespace tntdb
{
  namespace mysql
  {
    class RowContainer : public IResult
    {
        std::vector<SmartPtr<IRow, InternalRefCounted> > rows;

      public:
        void addRow(IRow* row)  { rows.push_back(row); }

        Row getRow(size_type tup_num) const;
        size_type size() const;
        size_type getFieldCount() const;
    };
  }
}

#endif // TNTDB_MYSQL_IMPL_ROWCONTAINER_H

