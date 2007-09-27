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

#ifndef TNTDB_MYSQL_IMPL_BOUNDVALUE_H
#define TNTDB_MYSQL_IMPL_BOUNDVALUE_H

#include <tntdb/iface/ivalue.h>
#include <tntdb/iface/irow.h>
#include <tntdb/smartptr.h>
#include <mysql.h>

namespace tntdb
{
  class Date;
  class Time;
  class Datetime;
  class Decimal;
  
  namespace mysql
  {
    class BoundValue : public IValue
    {
      public:
        typedef unsigned size_type;

      private:
        SmartPtr<IRow, InternalRefCounted> row;
        MYSQL_BIND& mysql_bind;

      public:
        BoundValue(IRow* row_, MYSQL_BIND& bind)
          : row(row_),
            mysql_bind(bind)
          { }

        virtual bool isNull() const;
        virtual bool getBool() const;
        virtual int getInt() const;
        virtual unsigned getUnsigned() const;
        virtual int32_t getInt32() const;
        virtual uint32_t getUnsigned32() const;
        virtual int64_t getInt64() const;
        virtual uint64_t getUnsigned64() const;
        virtual Decimal getDecimal() const;
        virtual float getFloat() const;
        virtual double getDouble() const;
        virtual char getChar() const;
        virtual void getString(std::string& ret) const;
        virtual Date getDate() const;
        virtual Time getTime() const;
        virtual Datetime getDatetime() const;
    };
  }
}

#endif // TNTDB_MYSQL_IMPL_BOUNDVALUE_H

