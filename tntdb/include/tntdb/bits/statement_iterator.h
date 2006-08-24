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
      SmartPtr<ICursor, InternalRefCounted> cursor;

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

