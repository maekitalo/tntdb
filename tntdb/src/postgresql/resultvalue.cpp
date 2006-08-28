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

#include <tntdb/postgresql/impl/resultvalue.h>
#include <tntdb/error.h>
#include <sstream>
#include <cxxtools/log.h>

log_define("tntdb.postgresql.resultvalue");

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
        throw TypeError(msg.str());
      }
      return ret;
    }

    int ResultValue::getInt() const
    {
      return getValue<int>(getString(), "int");
    }

    unsigned ResultValue::getUnsigned() const
    {
      return getValue<unsigned>(getString(), "unsigned");
    }

    float ResultValue::getFloat() const
    {
      return getValue<float>(getString(), "float");
    }

    double ResultValue::getDouble() const
    {
      return getValue<double>(getString(), "double");
    }

    char ResultValue::getChar() const
    {
      char* value = PQgetvalue(getPGresult(), row->getRowNumber(), tup_num);
      return *value;
    }

    std::string ResultValue::getString() const
    {
      char* value = PQgetvalue(getPGresult(), row->getRowNumber(), tup_num);
      int len = PQgetlength(getPGresult(), row->getRowNumber(), tup_num);
      return std::string(value, len);
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
      unsigned short hour, min, sec;
      std::istringstream in(value);
      in >> hour >> ch >> min >> ch >> sec;
      if (in)
        return Time(hour, min, sec);

      std::ostringstream msg;
      msg << "can't convert \"" << value << "\" to Time";
      throw TypeError(msg.str());
    }

    Datetime ResultValue::getDatetime() const
    {
      std::string value = PQgetvalue(getPGresult(), row->getRowNumber(), tup_num);
      if (value.find('-') != std::string::npos)
      {
        // ISO 8601/SQL standard
        char ch;
        unsigned short year, month, day, hour, min, sec;
        std::istringstream in(value);
        in >> year >> ch >> month >> ch >> day >> hour >> ch >> min >> ch >> sec;
        if (in)
          return Datetime(year, month, day, hour, min, sec);
      }
      else if (value.find('/') != std::string::npos)
      {
        // tradition style
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
