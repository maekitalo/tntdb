/*
 * Copyright (C) 2005 Tommi Maekitalo
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

#include <tntdb/impl/value.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/decimal.h>
#include <tntdb/blob.h>
#include <tntdb/error.h>
#include <sstream>
#include <cxxtools/convert.h>

namespace
{
  bool isTrue(char ch)
  {
    return ch == 't' || ch == 'T' || ch == 'y' || ch == 'Y' || ch == '1';
  }

  template <typename T>
  T getValue(const std::string& s, const char* tname)
  {
    try
    {
        return cxxtools::convert<T>(s);
    }
    catch (const cxxtools::ConversionError&)
    {
      std::ostringstream msg;
      msg << "can't convert \"" << s << "\" to " << tname;
      throw tntdb::TypeError(msg.str());
    }
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

  short ValueImpl::getShort() const
  {
    if (null)
      throw NullValue();
    return getValue<short>(data, "short");
  }

  int ValueImpl::getInt() const
  {
    if (null)
      throw NullValue();
    return getValue<int>(data, "int");
  }

  long ValueImpl::getLong() const
  {
    if (null)
      throw NullValue();
    return getValue<long>(data, "long");
  }

  unsigned short ValueImpl::getUnsignedShort() const
  {
    if (null)
      throw NullValue();
    return getValue<unsigned short>(data, "unsigned short");
  }

  unsigned ValueImpl::getUnsigned() const
  {
    if (null)
      throw NullValue();
    return getValue<unsigned>(data, "unsigned");
  }

  unsigned long ValueImpl::getUnsignedLong() const
  {
    if (null)
      throw NullValue();
    return getValue<unsigned long>(data, "unsigned long");
  }

  int32_t ValueImpl::getInt32() const
  {
    if (null)
      throw NullValue();
    return getValue<int32_t>(data, "int32_t");
  }

  uint32_t ValueImpl::getUnsigned32() const
  {
    if (null)
      throw NullValue();
    return getValue<uint32_t>(data, "uint32_t");
  }

  int64_t ValueImpl::getInt64() const
  {
    if (null)
      throw NullValue();
    return getValue<int64_t>(data, "int64_t");
  }

  uint64_t ValueImpl::getUnsigned64() const
  {
    if (null)
      throw NullValue();
    return getValue<uint64_t>(data, "uint64_t");
  }

  Decimal ValueImpl::getDecimal() const
  {
    if (null)
      throw NullValue();
    return getValue<Decimal>(data, "Decimal");
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

  void ValueImpl::getString(std::string& ret) const
  {
    if (null)
      throw NullValue();
    ret.assign(data);
  }

  void ValueImpl::getBlob(Blob& ret) const
  {
    if (null)
      throw NullValue();
    ret.assign(data.data(), data.size());
  }

  Date ValueImpl::getDate() const
  {
    if (null)
      throw NullValue();
    return Date::fromIso(data);
  }

  Time ValueImpl::getTime() const
  {
    if (null)
      throw NullValue();
    return Time::fromIso(data);
  }

  Datetime ValueImpl::getDatetime() const
  {
    if (null)
      throw NullValue();
    return Datetime::fromIso(data);
  }

}
