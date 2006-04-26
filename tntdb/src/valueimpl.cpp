/* 
   Copyright (C) 2005 Tommi Maekitalo

This file is part of tntdb.

Tntdb is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntdb is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntdb; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
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
