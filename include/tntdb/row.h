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

#ifndef TNTDB_ROW_H
#define TNTDB_ROW_H

#include <tntdb/bits/result.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/rowreader.h>
#include <tntdb/bits/row_iterator.h>

namespace tntdb
{
/// C++11 style _begin_ function for tntdb::Row.
inline Row::const_iterator begin(const Row& stmt)
{ return stmt.begin(); }

/// C++11 style _end_ function for tntdb::Row.
inline Row::const_iterator end(const Row& stmt)
{ return stmt.end(); }

}

#endif // TNTDB_ROW_H

