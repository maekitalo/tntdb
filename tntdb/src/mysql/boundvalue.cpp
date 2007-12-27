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

#include <tntdb/mysql/impl/boundvalue.h>
#include <tntdb/mysql/bindutils.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/decimal.h>
#include <cxxtools/log.h>

log_define("tntdb.mysql.boundvalue")

namespace tntdb
{
  namespace mysql
  {
    bool BoundValue::isNull() const
    {
      return mysql::isNull(mysql_bind);
    }

    bool BoundValue::getBool() const
    {
      return mysql::getBool(mysql_bind);
    }

    int BoundValue::getInt() const
    {
      return mysql::getInt(mysql_bind);
    }

    unsigned BoundValue::getUnsigned() const
    {
      return mysql::getUnsigned(mysql_bind);
    }

    int32_t BoundValue::getInt32() const
    {
      return mysql::getInt32(mysql_bind);
    }
    
    uint32_t BoundValue::getUnsigned32() const
    {
      return mysql::getUnsigned32(mysql_bind);
    }

    int64_t BoundValue::getInt64() const
    {
      return mysql::getInt64(mysql_bind);
    }
    
    uint64_t BoundValue::getUnsigned64() const
    {
      return mysql::getUnsigned64(mysql_bind);
    }
    
    Decimal BoundValue::getDecimal() const
    {
      return mysql::getDecimal(mysql_bind);
    }
    
    float BoundValue::getFloat() const
    {
      return mysql::getFloat(mysql_bind);
    }

    double BoundValue::getDouble() const
    {
      return mysql::getDouble(mysql_bind);
    }

    char BoundValue::getChar() const
    {
      return mysql::getChar(mysql_bind);
    }

    void BoundValue::getString(std::string& ret) const
    {
      mysql::getString(mysql_bind, ret);
    }

    void BoundValue::getBlob(Blob& ret) const
    {
      mysql::getBlob(mysql_bind, ret);
    }

    Date BoundValue::getDate() const
    {
      return mysql::getDate(mysql_bind);
    }

    Time BoundValue::getTime() const
    {
      return mysql::getTime(mysql_bind);
    }

    Datetime BoundValue::getDatetime() const
    {
      return mysql::getDatetime(mysql_bind);
    }

  }
}
