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

#ifndef TNTDB_BITS_STATEMENT_ITERATOR_H
#define TNTDB_BITS_STATEMENT_ITERATOR_H

#include <tntdb/iface/icursor.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/statement.h>

namespace tntdb
{
class RowReader;

/**
 * This class represents a database-cursor.
 */
class Statement::const_iterator
{
    Row current;
    std::shared_ptr<ICursor> cursor;

public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = int;
    using value_type = Row;
    using pointer = value_type*;
    using reference = value_type&;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;

    const_iterator() { }
    explicit const_iterator(std::shared_ptr<ICursor> cursor_);

    /**
     * Returns true, when the cursor points to the same iteration as the
     * passed iterator.
     */
    bool operator== (const const_iterator& c) const
      { return cursor == c.cursor; }
    /**
     * Returns true, when the cursor does not point to the same iteration as
     * the passed iterator.
     */
    bool operator!= (const const_iterator& c) const
      { return cursor != c.cursor; }

    /**
     * Fetches the next row. If no rows are available, the cursor is closed
     * and removed from this class.
     */
    const_iterator& operator++();
    const_iterator operator++(int);

    /**
     * Returns the current tntdb::Row-object.
     */
    const Row& operator* () const   { return current; }
    /**
     * This operator makes the const_iterator look like a pointer.
     */
    const Row* operator-> () const  { return &current; }

    /// Instantiates a tntdb::RowReader and fetches the value of the first column.
    template <typename T>
    RowReader get(T& ret) const;

    /// Instantiates a tntdb::RowReader and fetches the value of the first column and sets the null indicator.
    template <typename T>
    RowReader get(T& ret, bool& nullInd) const;

    /**
     * Returns the actual implementation-class.
     */
    const ICursor* getImpl() const  { return &*cursor; }
};

/// Alternative name for the statement iterator.
/// It may be easier to write and read.
typedef Statement::const_iterator Cursor;
}

#endif // TNTDB_BITS_STATEMENT_ITERATOR_H

