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

#include <tntdb/mysql/impl/rowvalue.h>
#include <tntdb/blob.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/decimal.h>
#include <sstream>

namespace tntdb
{
  namespace mysql
  {
    bool RowValue::isNull() const
    {
      return row[col] == 0;
    }

    bool RowValue::getBool() const
    {
      return row[col][0] == '1'
          || row[col][0] == 'y'
          || row[col][0] == 'Y'
          || row[col][0] == 't'
          || row[col][0] == 'T';
    }

    short RowValue::getShort() const
    {
      short ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    int RowValue::getInt() const
    {
      int ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    long RowValue::getLong() const
    {
      long ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    unsigned short RowValue::getUnsignedShort() const
    {
      unsigned short ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    unsigned RowValue::getUnsigned() const
    {
      unsigned ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    unsigned long RowValue::getUnsignedLong() const
    {
      unsigned long ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    int32_t RowValue::getInt32() const
    {
      int32_t ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    uint32_t RowValue::getUnsigned32() const
    {
      uint32_t ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    int64_t RowValue::getInt64() const
    {
      int64_t ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    uint64_t RowValue::getUnsigned64() const
    {
      uint64_t ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    Decimal RowValue::getDecimal() const
    {
      Decimal ret;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    float RowValue::getFloat() const
    {
      float ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    double RowValue::getDouble() const
    {
      double ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    char RowValue::getChar() const
    {
      return row[col][0];
    }

    void RowValue::getString(std::string& ret) const
    {
      ret.assign(row[col], len);
    }

    void RowValue::getBlob(Blob& ret) const
    {
      ret.assign(row[col], len);
    }

    Date RowValue::getDate() const
    {
      return Date::fromIso(getString());
    }

    Time RowValue::getTime() const
    {
      return Time::fromIso(getString());
    }

    Datetime RowValue::getDatetime() const
    {
      return Datetime::fromIso(getString());
    }

  }
}
