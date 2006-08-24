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

#include <tntdb/impl/value.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/error.h>
#include <cxxtools/log.h>
#include <sstream>

log_define("tntdb.value");

namespace
{
  bool isTrue(char ch)
  {
    return ch == 't' || ch == 'T' || ch == 'y' || ch == 'Y' || ch == '1';
  }

  template <typename T>
  T getValue(const std::string& s, const char* tname)
  {
    std::istringstream in(s);
    T ret;
    in >> ret;
    if (!in)
    {
      std::ostringstream msg;
      msg << "can't convert \"" << s << "\" to " << tname;
      throw tntdb::TypeError(msg.str());
    }
    return ret;
  }

}

namespace tntdb
{
  bool ValueImpl::isNull() const
  {
    return null;
  }

  bool ValueImpl::getBool() const
  {
    if (null)
      throw NullValue();
    return !data.empty()
        && isTrue(data.at(0));
  }

  int ValueImpl::getInt() const
  {
    if (null)
      throw NullValue();
    return getValue<int>(data, "int");
  }

  unsigned ValueImpl::getUnsigned() const
  {
    if (null)
      throw NullValue();
    return getValue<unsigned>(data, "unsigned");
  }

  float ValueImpl::getFloat() const
  {
    if (null)
      throw NullValue();
    return getValue<float>(data, "float");
  }

  double ValueImpl::getDouble() const
  {
    if (null)
      throw NullValue();
    return getValue<double>(data, "double");
  }

  char ValueImpl::getChar() const
  {
    if (null)
      throw NullValue();
    return data.at(0);
  }

  std::string ValueImpl::getString() const
  {
    if (null)
      throw NullValue();
    return data;
  }

  Date ValueImpl::getDate() const
  {
    if (null)
      throw NullValue();
    Date::fromIso(data);
  }

  Time ValueImpl::getTime() const
  {
    if (null)
      throw NullValue();
    Time::fromIso(data);
  }

  Datetime ValueImpl::getDatetime() const
  {
    if (null)
      throw NullValue();
    Datetime::fromIso(data);
  }

}
