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

#ifndef TNTDB_POSTGRESQL_IMPL_RESULTVALUE_H
#define TNTDB_POSTGRESQL_IMPL_RESULTVALUE_H

#include <tntdb/iface/ivalue.h>
#include <tntdb/postgresql/impl/resultrow.h>
#include <tntdb/bits/row.h>
#include <libpq-fe.h>

namespace tntdb
{
  namespace postgresql
  {
    class ResultValue : public IValue
    {
        tntdb::Row tntdbRow; // keeps a reference to the row
        ResultRow* row; // redundant type-safe pointer to tntdbRow
                        // (tntdbRow.getImpl() == row)
        int tup_num;

      public:
        ResultValue(ResultRow* row_, int tup_num_)
          : tntdbRow(row_),
            row(row_),
            tup_num(tup_num_)
        { }

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
        virtual void getBlob(Blob& ret) const;
        virtual Date getDate() const;
        virtual Time getTime() const;
        virtual Datetime getDatetime() const;

        std::string getString() const    { std::string ret; getString(ret); return ret; }
        PGresult* getPGresult() const    { return row->getPGresult(); }
    };
  }
}

#endif // TNTDB_POSTGRESQL_IMPL_RESULTVALUE_H

