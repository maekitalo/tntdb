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

#include <tntdb/mysql/bindutils.h>
#include <tntdb/mysql/error.h>
#include <tntdb/blob.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/decimal.h>
#include <cxxtools/log.h>
#include <sstream>
#include <string.h>

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

        case MYSQL_TYPE_INT24:
        {
          unsigned char* ptr = reinterpret_cast<unsigned char*>(bind.buffer);
          if (bind.is_unsigned)
          {
#if __BYTE_ORDER == __LITTLE_ENDIAN
            return (static_cast<int_type>(ptr[0]))
                 + (static_cast<int_type>(ptr[1]) << 8)
                 + (static_cast<int_type>(ptr[2]) << 16);
#else
            return (static_cast<int_type>(ptr[2]))
                 + (static_cast<int_type>(ptr[1]) << 8)
                 + (static_cast<int_type>(ptr[0]) << 16);
#endif
          }
          else
          {
#if __BYTE_ORDER == __LITTLE_ENDIAN
            if (ptr[2] < 128)
            {
              return static_cast<int_type>(ptr[0])
                  + (static_cast<int_type>(ptr[1]) << 8)
                  + (static_cast<int_type>(ptr[2]) << 16);
            }
            else
            {
              int32_t val;
              unsigned char* vptr = reinterpret_cast<unsigned char*>(&val);
              vptr[0] = ptr[0];
              vptr[1] = ptr[1];
              vptr[2] = ptr[2];
              vptr[3] = '\xff';
              return static_cast<int_type>(val);
            }
#else
            if (ptr[2] < 128)
            {
              return (static_cast<int_type>(ptr[2]))
                   + (static_cast<int_type>(ptr[1]) << 8)
                   + (static_cast<int_type>(ptr[0]) << 16);
            }
            else
            {
              int32_t val;
              unsigned char* vptr = reinterpret_cast<unsigned char*>(&val);
              vptr[0] = '\xff';
              vptr[1] = ptr[0];
              vptr[2] = ptr[1];
              vptr[3] = ptr[2];
              return static_cast<int_type>(val);
            }
#endif
          }
        }

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

        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_NEWDECIMAL:
        {
          std::string data(static_cast<char*>(bind.buffer), *bind.length);
          log_debug("extract integer-type from decimal \"" << data << '"');
          std::istringstream in(data);
          Decimal decimal;
          decimal.read(in);
          if (in.eof() || !in.fail())
          {
            int_type ret = decimal.getInteger<int_type>();
            return ret;
          }
          log_error("type-error in getInteger, type=" << bind.buffer_type);
          throw TypeError("type-error in getInteger");
        }
          
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
        {
          std::string data(static_cast<char*>(bind.buffer), *bind.length);
          log_debug("extract integer-type from string \"" << data << '"');
          std::istringstream in(data);
          int_type ret;
          in >> ret;
          if (in.eof() || !in.fail())
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
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_LONG:
          return getInteger<int>(bind);

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
          if (in.eof() || !in.fail())
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

    void setShort(MYSQL_BIND& bind, short data)
    {
      setValue(bind, data, MYSQL_TYPE_LONG);
      bind.is_unsigned = 0;
    }

    void setInt(MYSQL_BIND& bind, int data)
    {
      setValue(bind, data, MYSQL_TYPE_LONG);
      bind.is_unsigned = 0;
    }

    void setLong(MYSQL_BIND& bind, long data)
    {
      setValue(bind, data, MYSQL_TYPE_LONG);
      bind.is_unsigned = 0;
    }

    void setUnsignedShort(MYSQL_BIND& bind, unsigned short data)
    {
      setValue(bind, data, MYSQL_TYPE_LONG);
      bind.is_unsigned = 1;
    }

    void setUnsigned(MYSQL_BIND& bind, unsigned data)
    {
      setValue(bind, data, MYSQL_TYPE_LONG);
      bind.is_unsigned = 1;
    }

    void setUnsignedLong(MYSQL_BIND& bind, unsigned long data)
    {
      setValue(bind, data, MYSQL_TYPE_LONG);
      bind.is_unsigned = 1;
    }

    void setInt32(MYSQL_BIND& bind, int32_t data)
    {
      setValue(bind, data, MYSQL_TYPE_LONG);
      bind.is_unsigned = 0;
    }

    void setUnsigned32(MYSQL_BIND& bind, uint32_t data)
    {
      setValue(bind, data, MYSQL_TYPE_LONG);
      bind.is_unsigned = 1;
    }

    void setInt64(MYSQL_BIND& bind, int64_t data)
    {
      setValue(bind, data, MYSQL_TYPE_LONGLONG);
      bind.is_unsigned = 0;
    }

    void setUnsigned64(MYSQL_BIND& bind, uint64_t data)
    {
      setValue(bind, data, MYSQL_TYPE_LONGLONG);
      bind.is_unsigned = 1;
    }

    void setDecimal(MYSQL_BIND& bind, unsigned long& length, const Decimal& data)
    {
      std::ostringstream ds;
      ds.precision(24);
      data.print(ds, Decimal::infinityLong);
      std::string d = ds.str();
      reserve(bind, d.size());
      d.copy(static_cast<char*>(bind.buffer), d.size());
      bind.buffer_type = MYSQL_TYPE_NEWDECIMAL;
      bind.is_null = 0;
      length = d.size();
      bind.length = &length;
      bind.is_unsigned = 0;
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

      bind.buffer_type = MYSQL_TYPE_BLOB;
      bind.is_null = 0;
      bind.length = &length;
    }

    void setBlob(MYSQL_BIND& bind, unsigned long& length, const Blob& data)
    {
      length = data.size();
      reserve(bind, length);
      memcpy(static_cast<char*>(bind.buffer), data.data(), length);

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

    short getShort(const MYSQL_BIND& bind)
    {
      return getInteger<short>(bind);
    }

    int getInt(const MYSQL_BIND& bind)
    {
      return getInteger<int>(bind);
    }

    long getLong(const MYSQL_BIND& bind)
    {
      return getInteger<long>(bind);
    }

    unsigned short getUnsignedShort(const MYSQL_BIND& bind)
    {
      return getInteger<unsigned short>(bind);
    }

    unsigned getUnsigned(const MYSQL_BIND& bind)
    {
      return getInteger<unsigned>(bind);
    }

    unsigned long getUnsignedLong(const MYSQL_BIND& bind)
    {
      return getInteger<unsigned long>(bind);
    }

    int32_t getInt32(const MYSQL_BIND& bind)
    {
      return getInteger<int32_t>(bind);
    }

    uint32_t getUnsigned32(const MYSQL_BIND& bind)
    {
      return getInteger<uint32_t>(bind);
    }

    int64_t getInt64(const MYSQL_BIND& bind)
    {
      return getInteger<int64_t>(bind);
    }

    uint64_t getUnsigned64(const MYSQL_BIND& bind)
    {
      return getInteger<uint64_t>(bind);
    }

    Decimal getDecimal(const MYSQL_BIND& bind)
    {
      if (isNull(bind))
        throw NullValue();

      switch (bind.buffer_type)
      {
        case MYSQL_TYPE_TINY:
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_LONG:
        case MYSQL_TYPE_LONGLONG:
        {
          int64_t i = getInteger<int64_t>(bind);
          Decimal d(i);
          return d;
        }

        case MYSQL_TYPE_FLOAT:
        case MYSQL_TYPE_DOUBLE:
        {
          double f = getFloat<double>(bind);
          Decimal d(f);
          return d;
        }
          
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_NEWDECIMAL:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
        {
          std::string data(static_cast<char*>(bind.buffer), *bind.length);
          log_debug("extract Decimal from string \"" << data << '"');
          std::istringstream in(data);
          Decimal ret;
          in >> ret;
          if (in.eof() || !in.fail())
            return ret;

          // no break!!!
        }

        default:
          log_error("type-error in getDecimal, type=" << bind.buffer_type);
          throw TypeError("type-error in getDecimal");
      }
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

    void getString(const MYSQL_BIND& bind, std::string& ret)
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
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_NEWDECIMAL:
          ret.assign(static_cast<const char*>(bind.buffer),
                             *bind.length);
          break;

        case MYSQL_TYPE_DATE:
        {
          MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
          ret.assign(Date(ts->year, ts->month, ts->day).getIso());
          break;
        }

        case MYSQL_TYPE_TIME:
        {
          MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
          ret.assign(Time(ts->hour, ts->minute, ts->second).getIso());
          break;
        }

        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_TIMESTAMP:
        {
          MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(bind.buffer);
          ret.assign(Datetime(ts->year, ts->month, ts->day,
                          ts->hour, ts->minute, ts->second, ts->second_part).getIso());
          break;
        }

        default:
        {
          std::ostringstream s;
          switch (bind.buffer_type)
          {
            case MYSQL_TYPE_TINY:
            case MYSQL_TYPE_SHORT:
            case MYSQL_TYPE_INT24:
            case MYSQL_TYPE_LONG:
              if (bind.is_unsigned)
                s << getInteger<unsigned int>(bind);
              else
                s << getInteger<int>(bind);
              break;

            case MYSQL_TYPE_LONGLONG:
              if (bind.is_unsigned)
                s << *static_cast<long long unsigned*>(bind.buffer);
              else
                s << *static_cast<long long int*>(bind.buffer);
              break;

            case MYSQL_TYPE_FLOAT:
              s << *static_cast<float*>(bind.buffer);
              break;

            case MYSQL_TYPE_DOUBLE:
              s << *static_cast<double*>(bind.buffer);
              break;

            default:
              log_error("type-error in getString, type=" << bind.buffer_type);
              throw TypeError("type-error in getString");
          }
          ret.assign(s.str());
        }
      }
    }

    void getBlob(const MYSQL_BIND& bind, Blob& ret)
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
          ret.assign(static_cast<const char*>(bind.buffer),
                             *bind.length);
          break;

        default:
          log_error("type-error in getBlob, type=" << bind.buffer_type);
          throw TypeError("type-error in getBlob");
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
