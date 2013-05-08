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

#include <tntdb/oracle/multirow.h>
#include <tntdb/error.h>
#include <cxxtools/log.h>

log_define("tntdb.oracle.multirow")

namespace tntdb
{
  namespace oracle
  {
    MultiRow::MultiRow(Statement* stmt, unsigned rowcount)
    {
      ub4 columncount;

      sword ret = OCIAttrGet(stmt->getHandle(), OCI_HTYPE_STMT, &columncount,
        0, OCI_ATTR_PARAM_COUNT, stmt->getErrorHandle());
      stmt->checkError(ret, "OCIAttrGet(OCI_ATTR_PARAM_COUNT)");

      log_debug("define " << columncount << " parameters");
      _columns.resize(columncount);
      for (ub4 pos = 0; pos < columncount; ++pos)
        _columns[pos] = new MultiValue(stmt, pos, rowcount);
    }

    MultiRow::MultiRow(Statement* stmt, unsigned rowcount, unsigned columncount)
    {
      log_debug("define " << columncount << " parameters");
      _columns.resize(columncount);
      for (ub4 pos = 0; pos < columncount; ++pos)
        _columns[pos] = new MultiValue(stmt, pos, rowcount);
    }

    MultiValue::Ptr MultiRow::getValuesByNumber(unsigned field_num) const
    {
      return _columns.at(field_num);
    }

    MultiValue::Ptr MultiRow::getValuesByName(const std::string& field_name) const
    {
      return getValuesByNumber(getColIndexByName(field_name));
    }

    MultiRow::Columns::size_type MultiRow::getColIndexByName(const std::string& field_name) const
    {
      std::string field_name_upper;
      field_name_upper.reserve(field_name.size());
      for (std::string::const_iterator it = field_name.begin();
        it != field_name.end(); ++it)
          field_name_upper += std::toupper(*it);

      Columns::size_type i;
      for (i = 0; i < _columns.size(); ++i)
        if (_columns[i]->getColumnName() == field_name_upper)
          break;

      if (i == _columns.size())
        throw FieldNotFound(field_name);

      return i;
    }

    std::string MultiRow::getColumnName(unsigned field_num) const
    {
      return _columns.at(field_num)->getColumnName();
    }

  }
}
