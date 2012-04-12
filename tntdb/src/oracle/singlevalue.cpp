/*
 * Copyright (C) 2012 Tommi Maekitalo
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

#include <tntdb/oracle/singlevalue.h>
#include <cxxtools/log.h>

log_define("tntdb.oracle.singlevalue")

namespace tntdb
{
  namespace oracle
  {
    bool SingleValue::isNull() const
    {
      return _mv->isNull(_row);
    }

    bool SingleValue::getBool() const
    {
      return _mv->getBool(_row);
    }

    short SingleValue::getShort() const
    {
      return _mv->getShort(_row);
    }

    int SingleValue::getInt() const
    {
      return _mv->getInt(_row);
    }

    long SingleValue::getLong() const
    {
      return _mv->getLong(_row);
    }

    unsigned short SingleValue::getUnsignedShort() const
    {
      return _mv->getUnsignedShort(_row);
    }

    unsigned SingleValue::getUnsigned() const
    {
      return _mv->getUnsigned(_row);
    }

    unsigned long SingleValue::getUnsignedLong() const
    {
      return _mv->getUnsignedLong(_row);
    }

    int32_t SingleValue::getInt32() const
    {
      return _mv->getInt32(_row);
    }

    uint32_t SingleValue::getUnsigned32() const
    {
      return _mv->getUnsigned32(_row);
    }

    int64_t SingleValue::getInt64() const
    {
      return _mv->getInt64(_row);
    }

    uint64_t SingleValue::getUnsigned64() const
    {
      return _mv->getUnsigned64(_row);
    }

    Decimal SingleValue::getDecimal() const
    {
      return _mv->getDecimal(_row);
    }

    float SingleValue::getFloat() const
    {
      return _mv->getFloat(_row);
    }

    double SingleValue::getDouble() const
    {
      return _mv->getDouble(_row);
    }

    char SingleValue::getChar() const
    {
      return _mv->getChar(_row);
    }

    void SingleValue::getString(std::string& ret) const
    {
      return _mv->getString(_row, ret);
    }

    void SingleValue::getBlob(tntdb::Blob& ret) const
    {
      return _mv->getBlob(_row, ret);
    }

    Date SingleValue::getDate() const
    {
      return _mv->getDate(_row);
    }

    Time SingleValue::getTime() const
    {
      return _mv->getTime(_row);
    }

    tntdb::Datetime SingleValue::getDatetime() const
    {
      return _mv->getDatetime(_row);
    }

  }
}
