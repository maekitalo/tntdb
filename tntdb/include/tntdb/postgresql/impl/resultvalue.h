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
        virtual int getInt() const;
        virtual unsigned getUnsigned() const;
        virtual float getFloat() const;
        virtual double getDouble() const;
        virtual char getChar() const;
        virtual std::string getString() const;
        virtual Date getDate() const;
        virtual Time getTime() const;
        virtual Datetime getDatetime() const;

        PGresult* getPGresult() const    { return row->getPGresult(); }
    };
  }
}

#endif // TNTDB_POSTGRESQL_IMPL_RESULTVALUE_H

