/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#include <tntdb/oracle/row.h>
#include <tntdb/oracle/value.h>
#include <tntdb/error.h>
#include <cctype>
#include <cxxtools/log.h>

log_define("tntdb.oracle.row")

namespace tntdb
{
  namespace oracle
  {
    Row::Row(Statement* stmt)
    {
      ub4 columncount;

      sword ret = OCIAttrGet(stmt->getHandle(), OCI_HTYPE_STMT, &columncount,
        0, OCI_ATTR_PARAM_COUNT, stmt->getErrorHandle());
      stmt->checkError(ret, "OCIAttrGet(OCI_ATTR_PARAM_COUNT)");

      log_debug("define " << columncount << " parameters");
      values.reserve(columncount);
      for (ub4 pos = 0; pos < columncount; ++pos)
        values.push_back(tntdb::Value(new tntdb::oracle::Value(stmt, pos)));
    }

    Row::Row(Statement* stmt, unsigned columncount)
    {
      log_debug("define " << columncount << " parameters");
      values.reserve(columncount);
      for (ub4 pos = 0; pos < columncount; ++pos)
        values.push_back(tntdb::Value(new tntdb::oracle::Value(stmt, pos)));
    }

    Row::size_type Row::size() const
    {
      return values.size();
    }

    tntdb::Value Row::getValueByNumber(size_type field_num) const
    {
      return values.at(field_num);
    }

    tntdb::Value Row::getValueByName(const std::string& field_name) const
    {
      std::string field_name_upper;
      field_name_upper.reserve(field_name.size());
      for (std::string::const_iterator it = field_name.begin();
        it != field_name.end(); ++it)
          field_name_upper += std::toupper(*it);

      Values::const_iterator it;
      for (it = values.begin(); it != values.end(); ++it)
        if (static_cast<const Value*>(it->getImpl())->getColumnName() == field_name_upper)
          break;

      if (it == values.end())
        throw FieldNotFound(field_name);

      return *it;
    }

  }
}
