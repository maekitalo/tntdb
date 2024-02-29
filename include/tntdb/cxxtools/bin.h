/*
 * Copyright (C) 2023 Tommi Maekitalo
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

#ifndef TNTDB_CXXTOOLS_BIN_H
#define TNTDB_CXXTOOLS_BIN_H

#include <tntdb/statement.h>
#include <tntdb/value.h>
#include <cxxtools/bin/bin.h>
#include <sstream>

namespace tntdb
{

/**
  This operator is used to store binary data using cxxtools binary serializer
  in a database field.

  @code
    MyObject aSerializableObject = ...;
    tntdb::Statement ins = conn.prepare("insert into ... values (:col)");
    ins.set("col", cxxtools::Bin(aSerializableObject))
       .execute();
  @endcode
 */
template <typename T>
inline void operator<< (Hostvar& hostvar, const cxxtools::bin::BinOObject<T>& obj)
{
    std::ostringstream data;
    data << obj;
    hostvar.set(data.str());
}

/**
  This operator is used to retrieve bin data using cxxtools deserializer
  from a database field;

  @code
    MyObject aObject;
    tntdb::Statemnt sel = conn.prepare("select object from ...");
    sel.selectValue()
       .get(cxxtools::bin::Bin(aObject));
  @endcode

 */
template <typename T>
inline bool operator>> (const Value& value, cxxtools::bin::BinIOObject<T>&& obj)
{
    std::string str;
    if (!value.get(str))
        return false;

    std::istringstream data(str);
    data >> obj;
    return true;
}

}

#endif
