/* 
   Copyright (C) 2005 Tommi Maekitalo

This file is part of tntdb.

Tntdb is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntdb is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntdb; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
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

#endif // TNTDB_MYSQL_IMPL_BOUNDVALUE_H

