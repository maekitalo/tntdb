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

#ifndef TNTDB_IFACE_ISTATEMENT_H
#define TNTDB_IFACE_ISTATEMENT_H

#include <tntdb/refcounted.h>
#include <string>

namespace tntdb
{
  class Result;
  class Row;
  class Value;
  class Date;
  class Time;
  class Datetime;
  class Decimal;
  class ICursor;

  class IStatement : public RefCounted
  {
    public:
      typedef unsigned size_type;

      virtual void clear() = 0;

      virtual void setNull(const std::string& col) = 0;
      virtual void setBool(const std::string& col, bool data) = 0;
      virtual void setInt(const std::string& col, int data) = 0;
      virtual void setUnsigned(const std::string& col, unsigned data) = 0;
      virtual void setInt32(const std::string& col, int32_t data) = 0;
      virtual void setUnsigned32(const std::string& col, uint32_t data) = 0;
      virtual void setInt64(const std::string& col, int64_t data) = 0;
      virtual void setUnsigned64(const std::string& col, uint64_t data) = 0;
      virtual void setDecimal(const std::string& col, const Decimal& data) = 0;
      virtual void setFloat(const std::string& col, float data) = 0;
      virtual void setDouble(const std::string& col, double data) = 0;
      virtual void setChar(const std::string& col, char data) = 0;
      virtual void setString(const std::string& col, const std::string& data) = 0;
      virtual void setDate(const std::string& col, const Date& data) = 0;
      virtual void setTime(const std::string& col, const Time& data) = 0;
      virtual void setDatetime(const std::string& col, const Datetime& data) = 0;

      virtual size_type execute() = 0;
      virtual Result select() = 0;
      virtual Row selectRow() = 0;
      virtual Value selectValue() = 0;
      virtual ICursor* createCursor(unsigned fetchsize) = 0;
  };
}

#endif // TNTDB_IFACE_ISTATEMENT_H

