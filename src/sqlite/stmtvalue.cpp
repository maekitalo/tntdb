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

#include <tntdb/sqlite/impl/stmtvalue.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/decimal.h>
#include <tntdb/blob.h>
#include <tntdb/error.h>
#include <cxxtools/log.h>

log_define("tntdb.sqlite.stmtvalue")

namespace tntdb
{
  namespace sqlite
  {
    StmtValue::StmtValue(sqlite3_stmt* stmt_, const std::string& name_)
      : stmt(stmt_)
    {
      log_debug("sqlite3_column_count(" << stmt << ')');
      int count = ::sqlite3_column_count(stmt);

      for (iCol = 0; iCol < count; ++iCol)
      {
        log_debug("sqlite3_column_name(" << stmt << ", " << iCol << ')');
        const char* name = sqlite3_column_name(stmt, iCol);
        if (name == 0)
          throw std::bad_alloc();

        if (name == name_)
          break;
      }

      if (iCol >= count)
        throw FieldNotFound(name_);
    }

    bool StmtValue::isNull() const
    {
      log_debug("sqlite3_column_type(" << getStmt() << ", " << iCol << ')');
      return ::sqlite3_column_type(getStmt(), iCol) == SQLITE_NULL;
    }

    bool StmtValue::getBool() const
    {
      return getInt() != 0;
    }

    short StmtValue::getShort() const
    {
      return getInt();
    }

    int StmtValue::getInt() const
    {
      log_debug("sqlite3_column_int(" << getStmt() << ", " << iCol << ')');
      return ::sqlite3_column_int(getStmt(), iCol);
    }

    long StmtValue::getLong() const
    {
      return static_cast<long>(getInt64());
    }

    unsigned short StmtValue::getUnsignedShort() const
    {
      return static_cast<unsigned short>(getUnsigned());
    }

    unsigned StmtValue::getUnsigned() const
    {
      return static_cast<unsigned>(getInt64());
    }

    unsigned long StmtValue::getUnsignedLong() const
    {
      return static_cast<unsigned>(getInt64());
    }

    int32_t StmtValue::getInt32() const
    {
      return static_cast<int32_t>(getInt());
    }

    uint32_t StmtValue::getUnsigned32() const
    {
      return static_cast<uint32_t>(getInt64());
    }

    int64_t StmtValue::getInt64() const
    {
      log_debug("sqlite3_column_int64(" << getStmt() << ", " << iCol << ')');
      return ::sqlite3_column_int64(getStmt(), iCol);
    }

    uint64_t StmtValue::getUnsigned64() const
    {
      log_debug("possible loss of data in conversion from int64_t to uint64_t");
      return static_cast<uint64_t>(getInt64());
    }

    Decimal StmtValue::getDecimal() const
    {
      // SQLite 3.4.1 does not support the SQL decimal or numeric types.
      // So double is used instead, but of course binary floating point can
      // not accurately store decimal floating point numbers.
      log_debug("sqlite3_column_double(" << getStmt() << ", " << iCol << ')');
      return Decimal(::sqlite3_column_double(getStmt(), iCol));
    }

    float StmtValue::getFloat() const
    {
      return static_cast<float>(getDouble());
    }

    double StmtValue::getDouble() const
    {
      log_debug("sqlite3_column_double(" << getStmt() << ", " << iCol << ')');
      return ::sqlite3_column_double(getStmt(), iCol);
    }

    char StmtValue::getChar() const
    {
      log_debug("sqlite3_column_bytes(" << getStmt() << ", " << iCol << ')');
      int bytes = ::sqlite3_column_bytes(getStmt(), iCol);

      if (bytes <= 0)
        throw NullValue();

      log_debug("sqlite3_column_blob(" << getStmt() << ", " << iCol << ')');
      const void* ret = ::sqlite3_column_blob(getStmt(), iCol);
      return *static_cast<const char*>(ret);
    }

    void StmtValue::getString(std::string& ret) const
    {
      log_debug("sqlite3_column_bytes(" << getStmt() << ", " << iCol << ')');
      int bytes = ::sqlite3_column_bytes(getStmt(), iCol);

      if (bytes <= 0)
      {
        log_debug("empty string value - clear string");
        ret.clear();
      }
      else
      {
        log_debug("sqlite3_column_blob(" << getStmt() << ", " << iCol << ')');
        const void* data = ::sqlite3_column_blob(getStmt(), iCol);

        ret.assign(reinterpret_cast<const char*>(data), bytes);
      }
    }

    void StmtValue::getBlob(Blob& ret) const
    {
      log_debug("sqlite3_column_bytes(" << getStmt() << ", " << iCol << ')');
      int bytes = ::sqlite3_column_bytes(getStmt(), iCol);

      if (bytes <= 0)
      {
        log_debug("empty value - clear blob");
        ret.assign(0, 0);
      }
      else
      {
        log_debug("sqlite3_column_blob(" << getStmt() << ", " << iCol << ')');
        const void* data = ::sqlite3_column_blob(getStmt(), iCol);

        ret.assign(reinterpret_cast<const char*>(data), bytes);
      }
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
