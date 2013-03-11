/*
 * Copyright (C) 2012 Tommi Maekitalo
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

#include <tntdb/oracle/singlerow.h>
#include <cxxtools/log.h>

log_define("tntdb.oracle.singlerow")

namespace tntdb
{
  namespace oracle
  {
    SingleRow::SingleRow(MultiRow::Ptr mr, unsigned row)
      : _mr(mr),
        _row(row)
    {
        _values.resize(mr->size());
    }

    void SingleRow::row(unsigned r)
    {
      if (r != _row)
      {
        for (unsigned n = 0; n < _values.size(); ++n)
        {
          if (_values[n])
            _values[n]->row(r);
        }

        _row = r;
      }
    }

    SingleRow::size_type SingleRow::size() const
    {
      return _mr->size();
    }

    tntdb::Value SingleRow::getValueByNumber(size_type field_num) const
    {
      Values& v = const_cast<Values&>(_values);

      if (!v[field_num])
      {
        v[field_num] = new SingleValue(_mr->getValuesByNumber(field_num), _row);
      }

      return tntdb::Value(v[field_num].getPointer());
    }

    tntdb::Value SingleRow::getValueByName(const std::string& field_name) const
    {
      return getValueByNumber(_mr->getColIndexByName(field_name));
    }

    std::string SingleRow::getColumnName(size_type field_num) const
    {
      return _mr->getColumnName(field_num);
    }

  }
}
