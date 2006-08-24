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

#ifndef TNTDB_MYSQL_IMPL_ROWVALUE_H
#define TNTDB_MYSQL_IMPL_ROWVALUE_H

#include <tntdb/iface/ivalue.h>
#include <tntdb/bits/result.h>
#include <mysql.h>

namespace tntdb
{
  namespace mysql
  {
    class RowValue : public IValue
    {
      public:
        typedef unsigned size_type;

      private:
        Result result;
        MYSQL_ROW row;
        size_type col;
        size_type len;

      public:
        RowValue(const Result result_, MYSQL_ROW row_, size_type col_, size_type len_)
          : result(result_),
            row(row_),
            col(col_),
            len(len_)
            { }

        size_type size() const        { return len; }

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
    };
  }
}

#endif // TNTDB_MYSQL_IMPL_ROWVALUE_H

