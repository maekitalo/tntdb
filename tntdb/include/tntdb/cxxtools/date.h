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

#ifndef TNTDB_CXXTOOLS_DATE_H
#define TNTDB_CXXTOOLS_DATE_H

#include <tntdb/statement.h>
#include <tntdb/value.h>
#include <cxxtools/date.h>

namespace tntdb
{
  /**
    This operator can be used to pass a cxxtools::Date object to the
    database.

    @code
      tntdb::Statement stmt = conn.prepare("select ... where dt = :dt");
      cxxtools::Date dt(2011, 5, 3);
      stmt.set("dt", dt);
    @endcode
   */
  inline void operator<< (Hostvar& hostvar, const cxxtools::Date& dt)
  {
    hostvar.setDate(Date(dt.year(), dt.month(), dt.day()));
  }

  /**
    This operator can be used to retrieve a cxxtools::Date object from the
    database.

    @code
      tntdb::Statement sel = conn.prepare("select dt from ...");

      tntdb::Value v = sel.selectValue();

      cxxtools::Date dt;
      if (v.get(dt))
        std::cout << "dt=" << dt.toIsoString() << std::endl;
      else
        std::cout << "dt is null" << std::endl;
    @endcode

   */
  inline bool operator>> (const Value& value, cxxtools::Date& out)
  {
    if (value.isNull())
      return false;

    Date dt = value.getDate();
    out.set(dt.getYear(), dt.getMonth(), dt.getDay());
    return true;
  }

}

#endif // TNTDB_CXXTOOLS_DATE_H
