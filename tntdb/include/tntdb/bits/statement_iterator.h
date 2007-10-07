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

#ifndef TNTDB_BITS_STATEMENT_ITERATOR_H
#define TNTDB_BITS_STATEMENT_ITERATOR_H

#include <tntdb/iface/icursor.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/statement.h>
#include <iterator>

namespace tntdb
{
  /**
   * This class represents a database-cursor.
   */
  class Statement::const_iterator
      : public std::iterator<std::forward_iterator_tag, Row>
  {
      Row current;
      cxxtools::SmartPtr<ICursor> cursor;

    public:
      const_iterator(ICursor* cursor_ = 0);

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

      /**
       * Returns the actual implementation-class.
       */
      const ICursor* getImpl() const  { return &*cursor; }
  };
}

#endif // TNTDB_BITS_STATEMENT_ITERATOR_H

