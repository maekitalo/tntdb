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

#include <tntdb/mysql/bindvalues.h>
#include <tntdb/mysql/bindutils.h>
#include <tntdb/mysql/error.h>
#include <cxxtools/log.h>
#include <algorithm>

log_define("tntdb.mysql.bindvalues");

namespace tntdb
{
  namespace mysql
  {
    BindValues::BindValues()
      : valuesSize(0),
        values(0),
        bindAttributes(0)
    { }

    BindValues::BindValues(unsigned n)
      : valuesSize(n),
        values(new MYSQL_BIND[n]),
        bindAttributes(new BindAttributes[n])
    {
      ::memset(bindAttributes, 0, sizeof(BindAttributes) * valuesSize);
      ::memset(values, 0, sizeof(MYSQL_BIND) * valuesSize);
      for (unsigned n = 0; n < valuesSize; ++n)
      {
        values[n].length = &bindAttributes[n].length;
        values[n].is_null = &bindAttributes[n].isNull;
      }
    }

    BindValues::~BindValues()
    {
      if (values)
        for (unsigned n = 0; n < valuesSize; ++n)
          delete[] static_cast<char*>(values[n].buffer);

      delete[] values;
      delete[] bindAttributes;
    }

    void BindValues::setSize(unsigned n)
    {
      if (valuesSize == n)
        return;

      if (values)
      {
        for (unsigned nn = 0; nn < valuesSize; ++nn)
          delete[] static_cast<char*>(values[nn].buffer);
        delete[] values;
        delete[] bindAttributes;
      }

      valuesSize = n,
      values = new MYSQL_BIND[n];
      bindAttributes = new BindAttributes[n];

      ::memset(bindAttributes, 0, sizeof(BindAttributes) * valuesSize);
      ::memset(values, 0, sizeof(MYSQL_BIND) * valuesSize);
      for (unsigned n = 0; n < valuesSize; ++n)
      {
        values[n].length = &bindAttributes[n].length;
        values[n].is_null = &bindAttributes[n].isNull;
      }
    }

    void BindValues::initOutBuffer(unsigned n, MYSQL_FIELD& f)
    {
      log_debug("initOutBuffer name=" << f.name << " n=" << n << " length=" << f.length << " type=" << f.type
        << " max_length=" << f.max_length);

      reserve(values[n], std::max(f.length, f.max_length));
      if (f.type == 0)
        log_debug("no type in metadata for field " << n << "; using MYSQL_TYPE_VAR_STRING");
      values[n].buffer_type = f.type ? f.type : MYSQL_TYPE_VAR_STRING;
    }
  }
}
