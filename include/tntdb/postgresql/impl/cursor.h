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

#ifndef TNTDB_POSTGRESQL_IMPL_CURSOR_H
#define TNTDB_POSTGRESQL_IMPL_CURSOR_H

#include <tntdb/iface/icursor.h>
#include <tntdb/postgresql/impl/statement.h>
#include <tntdb/bits/statement.h>
#include <tntdb/bits/result.h>

namespace tntdb
{
  namespace postgresql
  {
    class Cursor : public ICursor
    {
        tntdb::Statement tntdbStmt;
        Statement* stmt;

        std::string cursorName;
        tntdb::Result currentResult;
        unsigned currentRow;

        unsigned fetchSize;

      public:
        Cursor(Statement* statement, unsigned fetchSize);
        ~Cursor();

        // method for ICursor
        Row fetch();

        // specific methods
        PGconn* getPGConn()            { return stmt->getPGConn(); }

        /**
         * Data is fetched in blocks. The fetchsize specifies, how many blocks
         * are fetched at once. This is fully transparent to the user, so there
         * is normally no need to change the default value, which is 100.
         */
        void setFetchSize(unsigned fs)  { fetchSize = fs; }
        unsigned getFetchSize() const   { return fetchSize; }
    };
  }
}

#endif // TNTDB_POSTGRESQL_IMPL_CURSOR_H

