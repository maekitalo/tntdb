/* 
 * Copyright (C) 2007 Tommi Maekitalo
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

#ifndef TNTDB_ORACLE_ROW_H
#define TNTDB_ORACLE_ROW_H

#include <tntdb/iface/irow.h>
#include <tntdb/value.h>
#include <tntdb/oracle/statement.h>
#include <vector>

namespace tntdb
{
  namespace oracle
  {
    class Row : public IRow
    {
        typedef std::vector<tntdb::Value> Values;
        Values values;

      public:
        explicit Row(Statement* stmt);
        Row(Statement* stmt, unsigned columncount);

        virtual size_type size() const;
        virtual tntdb::Value getValueByNumber(size_type field_num) const;
        virtual tntdb::Value getValueByName(const std::string& field_name) const;
    };
  }
}

#endif // TNTDB_ORACLE_ROW_H

