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

#ifndef TNTDB_IMPL_VALUE_H
#define TNTDB_IMPL_VALUE_H

#include <tntdb/iface/ivalue.h>

namespace tntdb
{
  class ValueImpl : public IValue
  {
      bool null;
      std::string data;

    public:
      ValueImpl()
        : null(true)
      { }

      ValueImpl(const std::string& d)
        : null(false),
          data(d)
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

#endif // TNTDB_IMPL_VALUE_H

