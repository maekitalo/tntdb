/*
 * Copyright (C) 2016 Tommi Maekitalo
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

#ifndef TNTDB_CXXTOOLS_TIMESPAN_H
#define TNTDB_CXXTOOLS_TIMESPAN_H

#include <tntdb/statement.h>
#include <tntdb/value.h>
#include <cxxtools/timespan.h>

namespace tntdb
{
  /**
    This operator can be used to pass a timespan object to the database.

    The operator expects a numeric column, which is set to the number of
    specified units.

    @code
      tntdb::Statement stmt = conn.prepare("select ... where ts = :ts");
      stmt.set("ts", cxxtools::Milliseconds(500));
    @endcode
   */
  inline void operator<< (Hostvar& hostvar, const cxxtools::Microseconds& ts)
  {
    hostvar.set(ts.totalUSecs());
  }

  inline void operator<< (Hostvar& hostvar, const cxxtools::Milliseconds& ts)
  {
    hostvar.set(Decimal(ts.totalUSecs(), -3));
  }

  inline void operator<< (Hostvar& hostvar, const cxxtools::Seconds& ts)
  {
    hostvar.set(Decimal(ts.totalUSecs(), -6));
  }

  inline void operator<< (Hostvar& hostvar, const cxxtools::Minutes& ts)
  {
    hostvar.set(Decimal(ts.totalUSecs() / 60, -6));
  }

  inline void operator<< (Hostvar& hostvar, const cxxtools::Hours& ts)
  {
    hostvar.set(Decimal(ts.totalUSecs() / 60 / 60, -6));
  }

  inline void operator<< (Hostvar& hostvar, const cxxtools::Days& ts)
  {
    hostvar.set(Decimal(ts.totalUSecs() / 60 / 60 / 24, -6));
  }

  /**
    This operator can be used to retrieve a timespan object from the
    database.

    A numeric value is read from the database and interpreted as a number
    of specified units.

    @code
      tntdb::Statement sel = conn.prepare("select tim from ...");

      tntdb::Value v = sel.selectValue();

      cxxtools::Seconds tim;
      if (v.get(tim))
        std::cout << "tim=" << tim << std::endl;
      else
        std::cout << "tim is null" << std::endl;
    @endcode

   */
  inline bool operator>> (const Value& value, cxxtools::Microseconds& out)
  {
    int64_t micros;
    if (!value.get(micros))
      return false;
    out = cxxtools::Timespan(micros);
    return true;
  }

  inline bool operator>> (const Value& value, cxxtools::Milliseconds& out)
  {
    Decimal ts;
    if (!value.get(ts))
      return false;
    int64_t micros = ts.getInteger<int64_t>(3);
    out = cxxtools::Timespan(micros);
    return true;
  }

  inline bool operator>> (const Value& value, cxxtools::Seconds& out)
  {
    Decimal ts;
    if (!value.get(ts))
      return false;
    int64_t micros = ts.getInteger<int64_t>(6);
    out = cxxtools::Timespan(micros);
    return true;
  }

  inline bool operator>> (const Value& value, cxxtools::Minutes& out)
  {
    Decimal ts;
    if (!value.get(ts))
      return false;
    int64_t v = ts.getInteger<int64_t>(9);
    out = cxxtools::Timespan(v*60/1000);
    return true;
  }

  inline bool operator>> (const Value& value, cxxtools::Hours& out)
  {
    Decimal ts;
    if (!value.get(ts))
      return false;
    int64_t v = ts.getInteger<int64_t>(9);
    out = cxxtools::Timespan(v*60*60/1000);
    return true;
  }

  inline bool operator>> (const Value& value, cxxtools::Days& out)
  {
    Decimal ts;
    if (!value.get(ts))
      return false;
    int64_t v = ts.getInteger<int64_t>(9);
    out = cxxtools::Timespan(v*60*60*24/1000);
    return true;
  }

}

#endif // TNTDB_CXXTOOLS_TIMESPAN_H
