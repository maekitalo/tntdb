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

#ifndef TNTDB_BITS_RESULT_ITERATOR_H
#define TNTDB_BITS_RESULT_ITERATOR_H

#include <tntdb/bits/result.h>
#include <tntdb/bits/row.h>
#include <iostream>

namespace tntdb
{
  /**
   * This implements a constant iterator to a result-set.
   */
  class Result::const_iterator
    : public std::iterator<std::random_access_iterator_tag, Row>
  {
    public:
      typedef const value_type& const_reference;
      typedef const value_type* const_pointer;

    private:
      Result result;
      Row current;
      size_type offset;

      void setOffset(size_type off)
      {
      if (off != offset)
      {
        offset = off;
        if (offset < result.size())
        current = result.getRow(offset);
      }
      }

    public:
      const_iterator(const Result& r, size_type off)
      : result(r),
        offset(off)
      {
        if (offset < r.size())
        current = r.getRow(offset);
      }

      /// Returns true, if the passed iterator points to the same row.
      /// The iterators should point to the same resultset. This is
      /// not checked. Only the offsets are considered.
      bool operator== (const const_iterator& it) const
      { return offset == it.offset; }

      /// Returns true, if the passed iterator points to different rows.
      /// The iterators should point to the same resultset. This is
      /// not checked. Only the offsets are considered.
      bool operator!= (const const_iterator& it) const
      { return !operator== (it); }

      /// Advances the iterator to the next element and returns the iterator to that element.
      const_iterator& operator++()
      { setOffset(offset + 1); return *this; }

      /// Advances the iterator to the next element and returns the previous iterator.
      const_iterator operator++(int)
      {
        const_iterator ret = *this;
        setOffset(offset + 1);
        return ret;
      }

      /// Moves the iterator to the previous element and returns the iterator to that element.
      const_iterator operator--()
      { setOffset(offset - 1); return *this; }

      /// Moves the iterator to the previous element and returns the previous iterator.
      const_iterator operator--(int)
      {
        const_iterator ret = *this;
        setOffset(offset - 1);
        return ret;
      }

      /// returns a reference to the current Row-object.
      const_reference operator*() const
      { return current; }

      /// returns a pointer to the current Row-object.
      const_pointer operator->() const
      { return &current; }

      /// Advances the iterator n elements and returns the iterator to that element.
      const_iterator& operator+= (difference_type n)
      {
        setOffset(offset + n);
        return *this;
      }

      /// Returns a iterator, which points n elements further.
      const_iterator operator+ (difference_type n) const
      {
        const_iterator it(*this);
        it += n;
        return it;
      }

      /// Moves the iterator n elements back and returns the iterator to that element.
      const_iterator& operator-= (difference_type n)
      {
        setOffset(offset - n);
        return *this;
      }

      /// Returns a iterator, which points n elements back.
      const_iterator operator- (difference_type n) const
      {
        const_iterator it(*this);
        it -= n;
        return it;
      }

      /// Returns the distance between 2 iterators.
      /// The iterators should point to the same resultset. This is
      /// not checked. Only the offsets are used.
      difference_type operator- (const const_iterator& it) const
      { return offset - it.offset; }
  };
}

#endif // TNTDB_BITS_RESULT_ITERATOR_H

