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

#ifndef TNTDB_POSTGRESQL_IMPL_RESULT_H
#define TNTDB_POSTGRESQL_IMPL_RESULT_H

#include <tntdb/iface/iresult.h>
#include <tntdb/bits/connection.h>
#include <libpq-fe.h>

namespace tntdb
{
  namespace postgresql
  {
    class Result : public IResult
    {
        tntdb::Connection conn;
        PGresult* result;

      public:
        Result(const tntdb::Connection& conn, PGresult* result);
        ~Result();

        PGresult* getPGresult() const  { return result; }

        Row getRow(size_type tup_num) const;
        size_type size() const;
        size_type getFieldCount() const;
    };
  }
}

#endif // TNTDB_POSTGRESQL_IMPL_RESULT_H

