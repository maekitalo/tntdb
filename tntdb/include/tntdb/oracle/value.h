/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#ifndef TNTDB_ORACLE_VALUE_H
#define TNTDB_ORACLE_VALUE_H

#include <tntdb/iface/ivalue.h>
#include <tntdb/oracle/datetime.h>
#include <tntdb/oracle/number.h>
#include <tntdb/oracle/blob.h>
#include <vector>
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

        std::vector<char> data;
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
        virtual short getShort() const;
        virtual int getInt() const;
        virtual long getLong() const;
        virtual unsigned short getUnsignedShort() const;
        virtual unsigned getUnsigned() const;
        virtual unsigned long getUnsignedLong() const;
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

