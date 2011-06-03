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

#ifndef TNTDB_MYSQL_BINDUTILS_H
#define TNTDB_MYSQL_BINDUTILS_H

#include <mysql.h>
#include <string>
#include <stdint.h>

namespace tntdb
{
  class Blob;
  class Date;
  class Time;
  class Datetime;
  class Decimal;
  
  namespace mysql
  {
    void setNull(MYSQL_BIND& value);
    void setBool(MYSQL_BIND& value, bool data);
    void setShort(MYSQL_BIND& value, short data);
    void setInt(MYSQL_BIND& value, int data);
    void setLong(MYSQL_BIND& value, long data);
    void setUnsignedShort(MYSQL_BIND& value, unsigned short data);
    void setUnsigned(MYSQL_BIND& value, unsigned data);
    void setUnsignedLong(MYSQL_BIND& value, unsigned long data);
    void setInt32(MYSQL_BIND& value, int32_t data);
    void setUnsigned32(MYSQL_BIND& value, uint32_t data);
    void setInt64(MYSQL_BIND& value, int64_t data);
    void setUnsigned64(MYSQL_BIND& value, uint64_t data);
    void setDecimal(MYSQL_BIND& value, unsigned long& length, const Decimal& data);
    void setFloat(MYSQL_BIND& value, float data);
    void setDouble(MYSQL_BIND& value, double data);
    void setChar(MYSQL_BIND& value, char data);
    void setString(MYSQL_BIND& value, unsigned long& length, const char* data);
    void setString(MYSQL_BIND& value, unsigned long& length, const std::string& data);
    void setBlob(MYSQL_BIND& value, unsigned long& length, const Blob& data);
    void setDate(MYSQL_BIND& value, const Date& data);
    void setTime(MYSQL_BIND& value, const Time& data);
    void setDatetime(MYSQL_BIND& value, const Datetime& data);

    bool           isNull(const MYSQL_BIND& value);
    bool           getBool(const MYSQL_BIND& value);
    short          getShort(const MYSQL_BIND& value);
    int            getInt(const MYSQL_BIND& value);
    long           getLong(const MYSQL_BIND& value);
    unsigned       getUnsigned(const MYSQL_BIND& value);
    unsigned short getUnsignedShort(const MYSQL_BIND& value);
    unsigned long  getUnsignedLong(const MYSQL_BIND& value);
    int32_t        getInt32(const MYSQL_BIND& value);
    uint32_t       getUnsigned32(const MYSQL_BIND& value);
    int64_t        getInt64(const MYSQL_BIND& value);
    uint64_t       getUnsigned64(const MYSQL_BIND& value);
    Decimal        getDecimal(const MYSQL_BIND& value);
    long           getLong(const MYSQL_BIND& value);
    float          getFloat(const MYSQL_BIND& value);
    double         getDouble(const MYSQL_BIND& value);
    char           getChar(const MYSQL_BIND& value);
    void           getString(const MYSQL_BIND& value, std::string& ret);
    void           getBlob(const MYSQL_BIND& value, Blob& ret);
    Date           getDate(const MYSQL_BIND& value);
    Time           getTime(const MYSQL_BIND& value);
    Datetime       getDatetime(const MYSQL_BIND& value);

    void transferValue(MYSQL_BIND& fromValue, MYSQL_BIND& toValue,
                       bool doRelease = true);
    void copyValue(const MYSQL_BIND& fromValue, MYSQL_BIND& toValue,
                   bool doRelease = true);
    void refValue(const MYSQL_BIND& fromValue, MYSQL_BIND& toValue,
                  bool doRelease = true);
    void releaseValue(MYSQL_BIND& value);

    void release(MYSQL_BIND& bind);
    void reserve(MYSQL_BIND& bind, unsigned long size);
    void reserveKeep(MYSQL_BIND& bind, unsigned long size);

    template <typename c_type>
    void setValue(MYSQL_BIND& bind, c_type value, enum_field_types mysql_type)
    {
      reserve(bind, sizeof(c_type));
      *static_cast<c_type*>(bind.buffer) = value;
      bind.buffer_type = mysql_type;
      bind.is_null = 0;
    }

  }
}

#endif // TNTDB_MYSQL_BINDUTILS_H

