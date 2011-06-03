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

#include <tntdb/postgresql/impl/resultvalue.h>
#include <tntdb/error.h>
#include <sstream>
#include <cxxtools/log.h>
#include <cxxtools/convert.h>
#include <limits>

log_define("tntdb.postgresql.resultvalue")

namespace tntdb
{
  namespace postgresql
  {
    bool ResultValue::isNull() const
    {
      return PQgetisnull(getPGresult(), row->getRowNumber(), tup_num) != 0;
    }

    bool ResultValue::getBool() const
    {
      char* value = PQgetvalue(getPGresult(), row->getRowNumber(), tup_num);
      return value[0] == 't' || value[0] == 'T'
          || value[0] == 'y' || value[0] == 'Y'
          || value[0] == '1';
    }

    short ResultValue::getShort() const
    {
      return cxxtools::convert<short>(getString());
    }

    int ResultValue::getInt() const
    {
      return cxxtools::convert<int>(getString());
    }

    long ResultValue::getLong() const
    {
      return cxxtools::convert<long>(getString());
    }

    unsigned short ResultValue::getUnsignedShort() const
    {
      return cxxtools::convert<unsigned short>(getString());
    }

    unsigned ResultValue::getUnsigned() const
    {
      return cxxtools::convert<unsigned>(getString());
    }

    unsigned long ResultValue::getUnsignedLong() const
    {
      return cxxtools::convert<unsigned long>(getString());
    }

    int32_t ResultValue::getInt32() const
    {
      return cxxtools::convert<int32_t>(getString());
    }

    uint32_t ResultValue::getUnsigned32() const
    {
      return cxxtools::convert<uint32_t>(getString());
    }

    int64_t ResultValue::getInt64() const
    {
      return cxxtools::convert<int64_t>(getString());
    }

    uint64_t ResultValue::getUnsigned64() const
    {
      return cxxtools::convert<uint64_t>(getString());
    }
    
    Decimal ResultValue::getDecimal() const
    {
      std::string s(getString());
      std::istringstream in(s);
      Decimal ret;
      // Ignore overflow errors while reading the fractional part of the
      // decimal number.
      ret.read(in, true);
      if (in.eof() || !in.fail())
        return ret;
      std::ostringstream msg;
      msg << "can't convert \"" << s << "\" to Decimal";
      throw TypeError(msg.str());
    }

    float ResultValue::getFloat() const
    {
      return cxxtools::convert<float>(getString());
    }

    double ResultValue::getDouble() const
    {
      return cxxtools::convert<double>(getString());
    }

    char ResultValue::getChar() const
    {
      char* value = PQgetvalue(getPGresult(), row->getRowNumber(), tup_num);
      return *value;
    }

    void ResultValue::getString(std::string& ret) const
    {
      if (PQgetisnull(getPGresult(), row->getRowNumber(), tup_num))
        throw NullValue();
      char* value = PQgetvalue(getPGresult(), row->getRowNumber(), tup_num);
      int len = PQgetlength(getPGresult(), row->getRowNumber(), tup_num);
      ret.assign(value, len);
    }

    void ResultValue::getBlob(Blob& ret) const
    {
      char* value = PQgetvalue(getPGresult(), row->getRowNumber(), tup_num);
      int len = PQgetlength(getPGresult(), row->getRowNumber(), tup_num);
      log_debug("PQgetlength returns " << len);
      size_t to_len;
      unsigned char* data = PQunescapeBytea(reinterpret_cast<unsigned char*>(value), &to_len);
      ret.assign(reinterpret_cast<char*>(data), to_len);
      PQfreemem(data);
    }

    Date ResultValue::getDate() const
    {
      std::string value = PQgetvalue(getPGresult(), row->getRowNumber(), tup_num);
      if (value.find('-') != std::string::npos)
      {
        // ISO 8601/SQL standard
        char ch;
        unsigned short year, month, day;
        std::istringstream in(value);
        in >> year >> ch >> month >> ch >> day;
        if (in)
          return Date(year, month, day);
      }
      else if (value.find('/') != std::string::npos)
      {
        // tradition style
        char ch;
        unsigned short year, month, day;
        std::istringstream in(value);
        in >> month >> ch >> day >> ch >> year;
        if (in)
          return Date(year, month, day);
      }
      else if (value.find('.') != std::string::npos)
      {
        // german/regional style
        char ch;
        unsigned short year, month, day;
        std::istringstream in(value);
        in >> day >> ch >> month >> ch >> year;
        if (in)
          return Date(year, month, day);
      }

      std::ostringstream msg;
      msg << "can't convert \"" << value << "\" to Date";
      throw TypeError(msg.str());
    }

    Time ResultValue::getTime() const
    {
      std::string value = PQgetvalue(getPGresult(), row->getRowNumber(), tup_num);
      char ch;
      unsigned short hour, min, sec, msec;
      float fsec;
      std::istringstream in(value);
      in >> hour >> ch >> min >> ch >> fsec;
      if (in)
      {
        fsec += .0005;
        sec = static_cast<unsigned short>(fsec);
        msec = static_cast<unsigned short>((fsec - sec) * 1000);
        return Time(hour, min, sec, msec);
      }

      std::ostringstream msg;
      msg << "can't convert \"" << value << "\" to Time";
      throw TypeError(msg.str());
    }

    Datetime ResultValue::getDatetime() const
    {
      std::string value = PQgetvalue(getPGresult(), row->getRowNumber(), tup_num);
      log_debug("datetime value=" << value);
      if (value.find('-') != std::string::npos)
      {
        // ISO 8601/SQL standard
        char ch;
        unsigned short year, month, day, hour, min, sec, msec;
        float fsec;
        std::istringstream in(value);
        in >> year >> ch >> month >> ch >> day >> hour >> ch >> min >> ch >> fsec;
        if (in)
        {
          fsec += .0005;
          sec = static_cast<unsigned short>(fsec);
          msec = static_cast<unsigned short>((fsec - sec) * 1000);
          return Datetime(year, month, day, hour, min, sec, msec);
        }
      }
      else if (value.find('/') != std::string::npos)
      {
        // traditional style
        char ch;
        unsigned short year, month, day, hour, min, sec;
        std::istringstream in(value);
        in >> month >> ch >> day >> ch >> year >> hour >> ch >> min >> ch >> sec;
        if (in)
          return Datetime(year, month, day, hour, min, sec);
      }
      else if (value.find('.') != std::string::npos)
      {
        // german/regional style
        char ch;
        unsigned short year, month, day, hour, min, sec;
        std::istringstream in(value);
        in >> day >> ch >> month >> ch >> year >> hour >> ch >> min >> ch >> sec;
        if (in)
          return Datetime(year, month, day, hour, min, sec);
      }

      std::ostringstream msg;
      msg << "can't convert \"" << value << "\" to Datetime";
      throw TypeError(msg.str());
    }

  }
}
