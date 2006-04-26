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

#ifndef TNTDB_POSTGRESQL_IMPL_RESULTROW_H
#define TNTDB_POSTGRESQL_IMPL_RESULTROW_H

#include <tntdb/bits/result.h>
#include <tntdb/iface/irow.h>
#include <libpq-fe.h>

namespace tntdb
{
  namespace postgresql
  {
    class Result;

    /// Row of a result-set of type Result
    class ResultRow : public IRow
    {
        tntdb::Result tntdbResult; // keeps a reference to the result
        Result* result; // redundant type-safe pointer to tntdbResult
                        // (tntdbResult.getImpl() == result)
        size_type rownumber;

      public:
        ResultRow(Result* result, size_type rownumber);
        ~ResultRow();

        unsigned size() const;
        Value getValue(size_type field_num) const;

        size_type getRowNumber() const   { return rownumber; }
        PGresult* getPGresult() const;
    };
  }
}

#endif // TNTDB_POSTGRESQL_IMPL_RESULTROW_H

