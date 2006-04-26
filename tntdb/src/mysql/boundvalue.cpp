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

#include <tntdb/mysql/impl/boundvalue.h>
#include <tntdb/mysql/bindutils.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <cxxtools/log.h>

log_define("tntdb.mysql.boundvalue");

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

    std::string BoundValue::getString() const
    {
      return mysql::getString(mysql_bind);
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
