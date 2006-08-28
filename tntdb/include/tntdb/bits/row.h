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

#ifndef TNTDB_BITS_ROW_H
#define TNTDB_BITS_ROW_H

#include <tntdb/iface/irow.h>
#include <tntdb/bits/value.h>
#include <tntdb/smartptr.h>

namespace tntdb
{
  /**
   * Row represents a row, which is fetched from the database.
   */
  class Row
  {
	public:
      /// Iterator for iterating through the values of this row.
	  class const_iterator;
	  typedef unsigned size_type;
	  typedef Value value_type;

	private:
	  SmartPtr<IRow, InternalRefCounted> row;

	public:
	  Row()  { }
	  Row(IRow* row_)
		: row(row_)
		{ }

      /// Returns the number of columns of this row.
	  unsigned size() const   { return row->size(); }
      /// Returns true, if this row-object has no columns.
	  bool empty() const      { return !row || size() == 0; }

      /// Returns the field_num's value-object.
	  Value getValue(size_type field_num) const
		{ return row->getValue(field_num); }
      /// Returns the field_num's value-object.
	  Value operator[] (size_type field_num) const
		{ return row->getValue(field_num); }

      /// Return true, if the specified value is null.
      bool isNull(size_type field_num) const
        { return getValue(field_num).isNull(); }
      //@{
      /**
       * The get-methods try to convert the value of the speicfied column to
       * the requested type.
       */

	  bool getBool(size_type field_num) const
        { return getValue(field_num).getBool(); }
	  int getInt(size_type field_num) const
        { return getValue(field_num).getInt(); }
	  unsigned getUnsigned(size_type field_num) const
        { return getValue(field_num).getUnsigned(); }
	  float getFloat(size_type field_num) const
        { return getValue(field_num).getFloat(); }
	  double getDouble(size_type field_num) const
        { return getValue(field_num).getDouble(); }
	  char getChar(size_type field_num) const
        { return getValue(field_num).getChar(); }
	  std::string getString(size_type field_num) const
        { return getValue(field_num).getString(); }
      //@}

      /// Returns a iterator to the first column
	  const_iterator begin() const;
      /// Returns a iterator past the last column
	  const_iterator end() const;

      /// Returns true, if this class is not connected to a actual databaserow.
      bool operator!() const         { return !row; }
      /// Returns the actual implementation-class.
      const IRow* getImpl() const    { return &*row; }
  };
}

#endif // TNTDB_BITS_ROW_H

