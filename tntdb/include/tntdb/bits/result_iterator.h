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

