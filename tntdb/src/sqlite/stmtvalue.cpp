/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#include <tntdb/sqlite/impl/stmtvalue.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <cxxtools/log.h>

log_define("tntdb.sqlite.stmtvalue");

namespace tntdb
{
  namespace sqlite
  {
    bool StmtValue::isNull() const
    {
      log_debug("sqlite3_column_type(" << getStmt() << ", " << iCol << ')');
      return ::sqlite3_column_type(getStmt(), iCol) == SQLITE_NULL;
    }

    bool StmtValue::getBool() const
    {
      return getInt() != 0;
    }

    int StmtValue::getInt() const
    {
      log_debug("sqlite3_column_int(" << getStmt() << ", " << iCol << ')');
      return ::sqlite3_column_int(getStmt(), iCol);
    }

    unsigned StmtValue::getUnsigned() const
    {
      log_debug("possible loss of data in conversion from int to unsigned");
      return static_cast<unsigned>(getInt());
    }

    float StmtValue::getFloat() const
    {
      return static_cast<float>(getInt());
    }

    double StmtValue::getDouble() const
    {
      log_debug("sqlite3_column_double(" << getStmt() << ", " << iCol << ')');
      return ::sqlite3_column_double(getStmt(), iCol);
    }

    char StmtValue::getChar() const
    {
      log_debug("sqlite3_column_text(" << getStmt() << ", " << iCol << ')');
      const unsigned char* ret = ::sqlite3_column_text(getStmt(), iCol);
      return static_cast<char>(*ret);
    }

    std::string StmtValue::getString() const
    {
      log_debug("sqlite3_column_bytes(" << getStmt() << ", " << iCol << ')');
      int bytes = ::sqlite3_column_bytes(getStmt(), iCol);
      log_debug("sqlite3_column_text(" << getStmt() << ", " << iCol << ')');
      const unsigned char* ret = ::sqlite3_column_text(getStmt(), iCol);
      return std::string(reinterpret_cast<const char*>(ret), bytes);
    }

    Date StmtValue::getDate() const
    {
      std::string str = getString();
      return Date::fromIso(str);
    }

    Time StmtValue::getTime() const
    {
      std::string str = getString();
      return Time::fromIso(str);
    }

    Datetime StmtValue::getDatetime() const
    {
      std::string str = getString();
      return Datetime::fromIso(str);
    }

  }
}
