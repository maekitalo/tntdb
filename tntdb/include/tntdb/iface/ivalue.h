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

#ifndef TNTDB_IFACE_IVALUE_H
#define TNTDB_IFACE_IVALUE_H

#include <cxxtools/refcounted.h>
#include <string>

namespace tntdb
{
  class Date;
  class Time;
  class Datetime;
  class Decimal;
  
  /// common inteface for resultvalues
  class IValue : public cxxtools::RefCounted
  {
    public:
      virtual bool isNull() const = 0;
      virtual bool getBool() const = 0;
      virtual int getInt() const = 0;
      virtual unsigned getUnsigned() const = 0;
      virtual int32_t getInt32() const = 0;
      virtual uint32_t getUnsigned32() const = 0;
      virtual int64_t getInt64() const = 0;
      virtual uint64_t getUnsigned64() const = 0;
      virtual Decimal getDecimal() const = 0;
      virtual float getFloat() const = 0;
      virtual double getDouble() const = 0;
      virtual char getChar() const = 0;
      virtual void getString(std::string& ret) const = 0;
      virtual Date getDate() const = 0;
      virtual Time getTime() const = 0;
      virtual Datetime getDatetime() const = 0;
  };
}

#endif // TNTDB_IFACE_IVALUE_H

