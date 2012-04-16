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

#ifndef TNTDB_ORACLE_MULTIVALUE_H
#define TNTDB_ORACLE_MULTIVALUE_H

#include <tntdb/oracle/datetime.h>
#include <tntdb/oracle/blob.h>
#include <tntdb/oracle/statement.h>
#include <tntdb/oracle/number.h>
#include <oci.h>
#include <cxxtools/refcounted.h>
#include <cxxtools/smartptr.h>

namespace tntdb
{
  namespace oracle
  {
    class MultiValue : public cxxtools::RefCounted
    {
        OCIDefine* _defp;
        mutable Connection* _conn;
        ub2 _type;
        ub2 _collen;
        ub2* _len;
        ub2* _nullind;
        unsigned _n;
        std::string _colName;

        // _data points to allocated data depending of _type:
        //
        //  _type              _data points to
        //  ------------------ -------------------------------
        //  SQLT_INT           long *
        //  SQLT_UIN           unsigned long *
        //  SQLT_FLT           double*
        //  SQLT_NUM           tntdb::oracle::Number*
        //  SQLT_VNU           tntdb::oracle::Number*
        //  SQLT_DAT           OCIDateTime*
        //  SQLT_TIMESTAMP     OCIDateTime*
        //  SQLT_TIMESTAMP_TZ  OCIDateTime*
        //  SQLT_TIMESTAMP_LTZ OCIDateTime*
        //  SQLT_BLOB          tntdb::oracle::Blob*
        //  other              char*

        char* _data;

        void init(Statement* stmt, OCIParam* paramp, ub4 pos);

        long longValue(unsigned n) const
        { return reinterpret_cast<long*>(_data)[n]; }
        unsigned long unsignedValue(unsigned n) const
        { return reinterpret_cast<unsigned long*>(_data)[n]; }
        double doubleValue(unsigned n) const
        { return reinterpret_cast<double*>(_data)[n]; }
        OCIDateTime* datetime(unsigned n) const
        { return reinterpret_cast<OCIDateTime**>(_data)[n]; }
        const OCINumber& number(unsigned n) const
        { return reinterpret_cast<const OCINumber*>(_data)[n]; }
        Decimal decimal(unsigned n) const
        { return Number::getDecimal(&number(n), _conn->getErrorHandle()); }
        OCILobLocator* blob(unsigned n) const
        { return reinterpret_cast<OCILobLocator**>(_data)[n]; }
        const char* data(unsigned n) const
        { return &_data[n * (_collen + 16)]; }

        // make non copyable:
        MultiValue(const MultiValue&)  { }
        MultiValue& operator= (MultiValue&)  { return *this; }

      public:
        typedef cxxtools::SmartPtr<MultiValue> Ptr;

        MultiValue(Statement* stmt, OCIParam* paramp_, ub4 pos, unsigned n);
        MultiValue(Statement* stmt, ub4 pos, unsigned n);
        ~MultiValue();

        unsigned size() const  { return _n; }

        bool isNull(unsigned n) const;
        bool getBool(unsigned n) const;
        short getShort(unsigned n) const;
        int getInt(unsigned n) const;
        long getLong(unsigned n) const;
        unsigned short getUnsignedShort(unsigned n) const;
        unsigned getUnsigned(unsigned n) const;
        unsigned long getUnsignedLong(unsigned n) const;
        int32_t getInt32(unsigned n) const;
        uint32_t getUnsigned32(unsigned n) const;
        int64_t getInt64(unsigned n) const;
        uint64_t getUnsigned64(unsigned n) const;
        Decimal getDecimal(unsigned n) const;
        float getFloat(unsigned n) const;
        double getDouble(unsigned n) const;
        char getChar(unsigned n) const;
        void getString(unsigned n, std::string& ret) const;
        void getBlob(unsigned n, tntdb::Blob& ret) const;
        Date getDate(unsigned n) const;
        Time getTime(unsigned n) const;
        tntdb::Datetime getDatetime(unsigned n) const;

        const std::string& getColumnName() const  { return _colName; }
    };
  }
}

#endif // TNTDB_ORACLE_MULTIVALUE_H

