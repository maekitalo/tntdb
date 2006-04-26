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

#ifndef TNTDB_BITS_RESULT_H
#define TNTDB_BITS_RESULT_H

#include <tntdb/smartptr.h>
#include <tntdb/iface/iresult.h>

namespace tntdb
{
  class Row;
  class Value;

  /**
   * The class holds a resultset of a query.
   * Querys might return multiple rows, which are accessable here in arbitary
   * order.
   */
  class Result
  {
	public:
	  typedef unsigned size_type;
	  typedef Row value_type;

	private:
	  SmartPtr<IResult, InternalRefCounted> result;

	public:
	  class const_iterator;

	  Result()  { }
	  Result(IResult* res)
		: result(res)
		{ }

      /**
       * Returns the row_num'ths row of the resultset.
       */
	  Row getRow(size_type row_num) const;
      /**
       * Returns the field_num'ths value of the row_num'ths row.
       */
	  Value getValue(size_type row_num, size_type field_num) const;

      /**
       * Returns the number of rows of this resultset.
       */
	  size_type size() const           { return result->size(); }
      /**
       * Returns true, when there are no rows available.
       */
	  bool empty() const               { return size() == 0; }
      /**
       * Returns the number of columns of the rows.
       */
	  size_type getFieldCount() const  { return result->getFieldCount(); }

      /**
       * Returns the row_num'ths row of the resultset.
       */
	  Row operator[] (size_type row_num) const;

      /**
       * Returns a const_iterator to the first row of this resultset.
       */
	  const_iterator begin() const;
      /**
       * Returns a const_iterator past the last row of this resultset.
       */
	  const_iterator end() const;

      /**
       * Returns true, when this classes does not reference a resultset.
       */
      bool operator!() const          { return !result; }
      /**
       * Returns the actual implementation-class.
       */
      const IResult* getImpl() const  { return &*result; }
  };
}

#endif // TNTDB_BITS_RESULT_H

