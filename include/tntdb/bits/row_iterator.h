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

#ifndef TNTDB_BITS_ROW_ITERATOR_H
#define TNTDB_BITS_ROW_ITERATOR_H

#include <tntdb/bits/row.h>
#include <tntdb/bits/value.h>

namespace tntdb
{
  class Row::const_iterator
    : public std::iterator<std::random_access_iterator_tag, Value>
  {
    public:
      typedef const value_type& const_reference;
      typedef const value_type* const_pointer;

    private:
      const IRow* row;
      size_type offset;
      Value current;

      void setOffset(size_type off)
      {
        if (off != offset)
        {
          offset = off;
          if (offset < row->size())
            current = Value(row->getValueByNumber(offset));
        }
      }

    public:
      const_iterator(const Row& row_, size_type offset_)
        : row(row_.getImpl()),
          offset(offset_)
      {
        if (offset < row->size())
          current = Value(row->getValueByNumber(offset));
      }

      bool operator== (const const_iterator& it) const
        { return offset == it.offset; }

      bool operator!= (const const_iterator& it) const
        { return !operator== (it); }

      const_iterator& operator++()
        { setOffset(offset + 1); return *this; }

      const_iterator operator++(int)
        {
          const_iterator ret = *this;
          setOffset(offset + 1);
          return ret;
        }

      const_iterator operator--()
        { setOffset(offset - 1); return *this; }

      const_iterator operator--(int)
        {
          const_iterator ret = *this;
          setOffset(offset - 1);
          return ret;
        }

      const_reference operator*() const
        { return current; }

      const_pointer operator->() const
        { return &current; }

      const_iterator& operator+= (difference_type n)
        {
          setOffset(offset + n);
          return *this;
        }

      const_iterator operator+ (difference_type n) const
        {
          const_iterator it(*this);
          it += n;
          return it;
        }

      const_iterator& operator-= (difference_type n)
        {
          setOffset(offset - n);
          return *this;
        }

      const_iterator operator- (difference_type n) const
        {
          const_iterator it(*this);
          it -= n;
          return it;
        }

      difference_type operator- (const const_iterator& it) const
        { return offset - it.offset; }
  };
}

#endif // TNTDB_BITS_ROW_ITERATOR_H

