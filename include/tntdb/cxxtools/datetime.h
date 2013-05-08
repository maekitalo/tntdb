/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef TNTDB_CXXTOOLS_DATETIME_H
#define TNTDB_CXXTOOLS_DATETIME_H

#include <tntdb/statement.h>
#include <tntdb/value.h>
#include <cxxtools/datetime.h>

namespace tntdb
{
  /**
    This operator can be used to pass a cxxtools::DateTime object to the
    database.

    @code
      tntdb::Statement stmt = conn.prepare("select ... where dt = :dt");
      cxxtools::DateTime dt(2011, 5, 3, 18, 45, 30);
      stmt.set("dt", dt);
    @endcode
   */
  inline void operator<< (Hostvar& hostvar, const cxxtools::DateTime& dt)
  {
    hostvar.setDatetime(Datetime(dt.year(), dt.month(), dt.day(),
      dt.hour(), dt.minute(), dt.second(), dt.msec()));
  }

  /**
    This operator can be used to retrieve a cxxtools::DateTime object from the
    database.

    @code
      tntdb::Statement sel = conn.prepare("select dt from ...");

      tntdb::Value v = sel.selectValue();

      cxxtools::DateTime dt;
      if (v.get(dt))
        std::cout << "dt=" << dt.toIsoString() << std::endl;
      else
        std::cout << "dt is null" << std::endl;
    @endcode

   */
  inline bool operator>> (const Value& value, cxxtools::DateTime& out)
  {
    if (value.isNull())
      return false;

    Datetime dt = value.getDatetime();
    out.set(dt.getYear(), dt.getMonth(), dt.getDay(),
            dt.getHour(), dt.getMinute(), dt.getSecond(),
            dt.getMillis());
    return true;
  }

}

#endif // TNTDB_CXXTOOLS_DATETIME_H
