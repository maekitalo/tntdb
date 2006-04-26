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

#include <tntdb/mysql/bindutils.h>
#include <tntdb/mysql/error.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <cxxtools/log.h>
#include <sstream>

log_define("tntdb.mysql.bindutils")

namespace tntdb
{
  namespace mysql
  {
    void release(MYSQL_BIND& bind)
    {
      log_debug("release buffer");
      delete[] static_cast<char*>(bind.buffer);
      bind.buffer = 0;
      bind.buffer_length = 0;
      bind.is_null = 0;
    }

    void reserve(MYSQL_BIND& bind, unsigned long size)
    {
      // At least for timestamp-data metadata returns a too small size-value,
      // so we grow it to at least 64 bytes. I don't know if this is a bug in
      // mysql or here.
      if (size < 64)
        size = 64;

      if (bind.buffer_length < size)
      {
        log_debug("grow buffer to " << size << " initial " << bind.buffer_length);

        delete[] static_cast<char*>(bind.buffer);
        bind.buffer = new char[size];
        bind.buffer_length = size;
      }
    }

    void reserveKeep(MYSQL_BIND& bind, unsigned long size)
    {
      if (bind.buffer_length < size)
      {
        log_debug("grow buffer to " << size << " initial " << bind.buffer_length);
        char* b = new char[size + 1];
        if (bind.buffer)
        {
          ::memcpy(b, bind.buffer, bind.buffer_length);
          delete[] static_cast<char*>(bind.buffer);
        }
        bind.buffer = b;
        bind.buffer_length = size;
      }
    }

    template <typename int_type>
    int_type getInteger(const MYSQL_BIND& bind)
    {
      if (isNull(bind))
        throw NullValue();

      switch (bind.buffer_type)
      {
        case MYSQL_TYPE_TINY:
          if (bind.is_unsigned)
            return *static_cast<unsigned char*>(bind.buffer);
          else
            return *static_cast<signed char*>(bind.buffer);

        case MYSQL_TYPE_SHORT:
          if (bind.is_unsigned)
            return *static_cast<unsigned short int*>(bind.buffer);
          else
            return *static_cast<short int*>(bind.buffer);

        case MYSQL_TYPE_LONG:
          if (bind.is_unsigned)
            return *static_cast<unsigned int*>(bind.buffer);
          else
            return *static_cast<int*>(bind.buffer);

        case MYSQL_TYPE_LONGLONG:
          if (bind.is_unsigned)
            return *static_cast<long long unsigned*>(bind.buffer);
          else
            return *static_cast<long long int*>(bind.buffer);

        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
        {
          std::string data(static_cast<char*>(bind.buffer), *bind.length);
          log_debug("extract integer-type from string \"" << data << '"');
          std::istringstream in(data);
          int_type ret;
          in >> ret;
          if (in)
            return ret;

          // no break!!!
        }

        default:
          log_error("type-error in getInteger, type=" << bind.buffer_type);
          throw TypeError("type-error in getInteger");
      }
    }

    template <typename float_type>
    float_type getFloat(const MYSQL_BIND& bind)
    {
      if (isNull(bind))
        throw NullValue();

      switch (bind.buffer_type)
      {
        case MYSQL_TYPE_TINY:
          if (bind.is_unsigned)
            return *static_cast<unsigned char*>(bind.buffer);
          else
            return *static_cast<signed char*>(bind.buffer);

        case MYSQL_TYPE_SHORT:
          if (bind.is_unsigned)
            return *static_cast<unsigned short int*>(bind.buffer);
          else
            return *static_cast<short int*>(bind.buffer);

        case MYSQL_TYPE_LONG:
          if (bind.is_unsigned)
            return *static_cast<unsigned int*>(bind.buffer);
          else
            return *static_cast<int*>(bind.buffer);

        case MYSQL_TYPE_FLOAT:
          return *static_cast<float*>(bind.buffer);

        case MYSQL_TYPE_DOUBLE:
          return *static_cast<double*>(bind.buffer);

        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
        {
          std::string data(static_cast<char*>(bind.buffer), *bind.length);
          log_debug("extract float-type from string \"" << data << '"');
          std::istringstream in(data);
          float_type ret;
          in >> ret;
          if (in)
            return ret;

          // no break!!!
        }

        default:
          log_error("type-error in getFloat, type=" << bind.buffer_type);
          throw TypeError("type-error in getFloat");
      }
    }

    void setNull(MYSQL_BIND& bind)
    {
      release(bind);
      bind.buffer_type = MYSQL_TYPE_NULL;
    }

    void setBool(MYSQL_BIND& bind, bool data)
    {
      setValue(bind, static_cast<short int>(data), MYSQL_TYPE_SHORT);
      bind.is_unsigned = 1;
    }

    void setInt(MYSQL_BIND& bind, int data)
    {
      setValue(bind, data, MYSQL_TYPE_LONG);
      bind.is_unsigned = 0;
    }

    void setUnsigned(MYSQL_BIND& bind, unsigned data)
    {
      setValue(bind, data, MYSQL_TYPE_LONG);
      bind.is_unsigned = 1;
    }

    void setFloat(MYSQL_BIND& bind, float data)
    {
      setValue(bind, data, MYSQL_TYPE_FLOAT);
    }

    void setDouble(MYSQL_BIND& bind, double data)
    {
      setValue(bind, data, MYSQL_TYPE_DOUBLE);
    }

    void setChar(MYSQL_BIND& bind, char data)
    {
      setValue(bind, data, MYSQL_TYPE_TINY);
    }

    void setString(MYSQL_BIND& bind, unsigned long& length, const char* data)
    {
      length = ::strlen(data);
      reserve(bind, length + 1);
      memcpy(static_cast<char*>(bind.buffer), data, length + 1);

      bind.buffer_type = MYSQL_TYPE_VAR_STRING;
      bind.is_null = 0;
      bind.length = &length;
    }

    void setString(MYSQL_BIND& bind, unsigned long& length,
      const std::string& data)
    {
      reserve(bind, data.size());
      data.copy(static_cast<char*>(bind.buffer), data.size());

      bind.buffer_type = MYSQL_TYPE_VAR_STRING;
      bind.is_null = 0;
      length = data.size();
      bind.length = &length;
    }

    void setDate(MYSQL_BIND& bind, const Date& data)
    {
      reserve(bind, sizeof(MYSQL_TIME));
      memset(bind.buffer, 0, sizeof(MYSQL_TIME));

      MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
      ts->year = data.getYear();
      ts->month = data.getMonth();
      ts->day = data.getDay();

      bind.buffer_type = MYSQL_TYPE_DATE;
      bind.is_null = 0;
    }

    void setTime(MYSQL_BIND& bind, const Time& data)
    {
      reserve(bind, sizeof(MYSQL_TIME));
      memset(bind.buffer, 0, sizeof(MYSQL_TIME));

      MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
      ts->hour = data.getHour();
      ts->minute = data.getMinute();
      ts->second = data.getSecond();

      bind.buffer_type = MYSQL_TYPE_DATE;
      bind.is_null = 0;
    }

    void setDatetime(MYSQL_BIND& bind, const Datetime& data)
    {
      reserve(bind, sizeof(MYSQL_TIME));
      memset(bind.buffer, 0, sizeof(MYSQL_TIME));

      MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
      ts->year = data.getYear();
      ts->month = data.getMonth();
      ts->day = data.getDay();
      ts->hour = data.getHour();
      ts->minute = data.getMinute();
      ts->second = data.getSecond();

      bind.buffer_type = MYSQL_TYPE_DATE;
      bind.is_null = 0;
    }

    bool isNull(const MYSQL_BIND& bind)
    {
      return bind.buffer_type == MYSQL_TYPE_NULL
          || bind.is_null && *bind.is_null;
    }

    bool getBool(const MYSQL_BIND& bind)
    {
      return getInteger<bool>(bind);
    }

    int getInt(const MYSQL_BIND& bind)
    {
      return getInteger<int>(bind);
    }

    unsigned getUnsigned(const MYSQL_BIND& bind)
    {
      return getInteger<unsigned>(bind);
    }

    long getLong(const MYSQL_BIND& bind)
    {
      return getInteger<long>(bind);
    }

    float getFloat(const MYSQL_BIND& bind)
    {
      return getFloat<float>(bind);
    }

    double getDouble(const MYSQL_BIND& bind)
    {
      return getFloat<double>(bind);
    }

    char getChar(const MYSQL_BIND& bind)
    {
      if (isNull(bind))
        throw NullValue();

      switch (bind.buffer_type)
      {
        case MYSQL_TYPE_TINY:
          if (bind.is_unsigned)
            return *static_cast<unsigned char*>(bind.buffer);
          else
            return *static_cast<signed char*>(bind.buffer);

        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VAR_STRING:
          if (*bind.length > 0)
            return *static_cast<char*>(bind.buffer);
          // no break!

        default:
          log_error("type-error in getChar, type=" << bind.buffer_type);
          throw TypeError("type-error in getChar");
      }
    }

    std::string getString(const MYSQL_BIND& bind)
    {
      if (isNull(bind))
        throw NullValue();

      switch (bind.buffer_type)
      {
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
          return std::string(static_cast<const char*>(bind.buffer),
                             *bind.length);

        case MYSQL_TYPE_DATE:
        {
          MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
          return Date(ts->year, ts->month, ts->day).getIso();
        }

        case MYSQL_TYPE_TIME:
        {
          MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
          return Time(ts->hour, ts->minute, ts->second).getIso();
        }

        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_TIMESTAMP:
        {
          MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
          return Datetime(ts->year, ts->month, ts->day,
                          ts->hour, ts->minute, ts->second, ts->second_part).getIso();
        }

        default:
        {
          std::ostringstream ret;
          switch (bind.buffer_type)
          {
            case MYSQL_TYPE_TINY:
              if (bind.is_unsigned)
                ret << static_cast<unsigned>(*static_cast<unsigned char*>(bind.buffer));
              else
                ret << static_cast<int>(*static_cast<signed char*>(bind.buffer));
              break;

            case MYSQL_TYPE_SHORT:
              if (bind.is_unsigned)
                ret << *static_cast<unsigned short int*>(bind.buffer);
              else
                ret << *static_cast<short int*>(bind.buffer);
              break;

            case MYSQL_TYPE_LONG:
              if (bind.is_unsigned)
                ret << *static_cast<unsigned int*>(bind.buffer);
              else
                ret << *static_cast<int*>(bind.buffer);
              break;

            case MYSQL_TYPE_LONGLONG:
              if (bind.is_unsigned)
                ret << *static_cast<long long unsigned*>(bind.buffer);
              else
                ret << *static_cast<long long int*>(bind.buffer);
              break;

            case MYSQL_TYPE_FLOAT:
              ret << *static_cast<float*>(bind.buffer);
              break;

            case MYSQL_TYPE_DOUBLE:
              ret << *static_cast<double*>(bind.buffer);
              break;

            default:
              log_error("type-error in getString, type=" << bind.buffer_type);
              throw TypeError("type-error in getString");
          }
          return ret.str();
        }
      }
    }

    Date getDate(const MYSQL_BIND& bind)
    {
      if (isNull(bind))
        throw NullValue();

      switch (bind.buffer_type)
      {
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_TIMESTAMP:
        {
          MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
          return Date(ts->year, ts->month, ts->day);
        }

        default:
          log_error("type-error in getDate, type=" << bind.buffer_type);
          throw TypeError("type-error in getDate");
      }
    }

    Time getTime(const MYSQL_BIND& bind)
    {
      if (isNull(bind))
        throw NullValue();

      switch (bind.buffer_type)
      {
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_TIMESTAMP:
        {
          MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
          return Time(ts->hour, ts->minute, ts->second);
        }

        default:
          log_error("type-error in getTime, type=" << bind.buffer_type);
          throw TypeError("type-error in getTime");
      }
    }

    Datetime getDatetime(const MYSQL_BIND& bind)
    {
      if (isNull(bind))
        throw NullValue();

      switch (bind.buffer_type)
      {
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_TIMESTAMP:
        {
          MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
          return Datetime(ts->year, ts->month, ts->day,
                          ts->hour, ts->minute, ts->second);
        }

        default:
          log_error("type-error in getDatetime, type=" << bind.buffer_type);
          throw TypeError("type-error in getDatetime");
      }
    }

    void transferValue(MYSQL_BIND& fromValue, MYSQL_BIND& toValue,
      bool doRelease)
    {
      if (doRelease)
        release(toValue);

      toValue.buffer = fromValue.buffer;
      toValue.buffer_type = fromValue.buffer_type;
      toValue.buffer_length = fromValue.buffer_length;
      toValue.is_null = fromValue.is_null;
      toValue.length = fromValue.length;

      setNull(fromValue);
    }

    void copyValue(const MYSQL_BIND& fromValue, MYSQL_BIND& toValue,
      bool doRelease)
    {
      if (doRelease)
        release(toValue);

      toValue.buffer = 0;
      reserveKeep(toValue, fromValue.buffer_length);

      toValue.buffer_type = fromValue.buffer_type;
      toValue.buffer_length = fromValue.buffer_length;
      toValue.is_null = fromValue.is_null;
      toValue.length = fromValue.length;
    }

    void refValue(const MYSQL_BIND& fromValue, MYSQL_BIND& toValue,
      bool doRelease)
    {
      if (doRelease)
        release(toValue);

      toValue.buffer = fromValue.buffer;
      toValue.buffer_type = fromValue.buffer_type;
      toValue.buffer_length = fromValue.buffer_length;
      toValue.is_null = fromValue.is_null;
      toValue.length = fromValue.length;
    }

    void releaseValue(MYSQL_BIND& value)
    {
      release(value);
    }
  }
}
