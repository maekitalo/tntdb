/* 
 * Copyright (C) 2007 Tommi Maekitalo
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

#ifndef TNTDB_ORACLE_VALUE_H
#define TNTDB_ORACLE_VALUE_H

#include <tntdb/iface/ivalue.h>
#include <tntdb/oracle/datetime.h>
#include <tntdb/oracle/number.h>
#include <tntdb/oracle/blob.h>
#include <cxxtools/dynbuffer.h>
#include <oci.h>

namespace tntdb
{
  namespace oracle
  {
    class Statement;

    class Value : public IValue
    {
        OCIDefine* defp;
        OCIParam* paramp;
        ub2 len;
        ub2 type;
        ub2 nullind;

        cxxtools::Dynbuffer<char> data;
        Datetime datetime;
        long longValue;
        uint32_t uint32Value;
        double doubleValue;
        Number number;
        Blob blob;

        std::string colName;

        void init(Statement* stmt, OCIParam* paramp_, ub4 pos);

      public:
        Value(Statement* stmt, OCIParam* paramp_, ub4 pos);
        Value(Statement* stmt, ub4 pos);
        ~Value();

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
        virtual void getBlob(tntdb::Blob& ret) const;
        virtual Date getDate() const;
        virtual Time getTime() const;
        virtual tntdb::Datetime getDatetime() const;

        const std::string& getColumnName() const  { return colName; }
    };
  }
}

#endif // TNTDB_ORACLE_VALUE_H

