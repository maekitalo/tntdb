/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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

