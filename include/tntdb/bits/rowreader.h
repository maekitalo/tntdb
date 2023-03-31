/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef TNTDB_ROWREADER_H
#define TNTDB_ROWREADER_H

#include <tntdb/bits/row.h>

namespace tntdb
{
/**
 * A RowReader is a class which helps reading multiple columns from a _row.
 *
 * This class helds a field number counter, which is incremented each time
 * a value is fetched from the underlying _row using one of the get methods.
 * This class is normally instantiated implicitly using the tntdb::Row::reader
 * method.
 *
 * The get methods return a reference to the _row reader to make chaining of
 * calls easy.
 *
 * Example:
 * \code
 *   tntdb::Statement s = conn.prepare("select col1, col2, col3 from table");
 *   for (tntdb::Statement::const_iterator cur = s.begin(); cur != s.end(); ++cur)
 *   {
 *     int col1;
 *     std::string col2;
 *     long col3;
 *     bool col3IsNotNull;
 *     // note that a dereferenced cursor returns a tntdb::Row:
 *     cur->reader().get(col1)       // this fetches the first value and
 *                                   // increments the field counter
 *                  .get(col2)       // and the second
 *                  .get(col3, col3IsNotNull);  // and this reads the 3rd column
 *                                              // and a flag, if it is not null
 *     // ...
 *     // do whatever you need to do with the values here
 *     // ...
 *   }
 * \endcode
 */
class RowReader
{
    const Row& _row;
    Row::size_type _fieldNum;

public:
    /// instatiates a _row reader with a _row and a initial field counter
    explicit RowReader(const Row& row_, Row::size_type field_num_ = 0)
      : _row(row_),
        _fieldNum(field_num_)
    { }

    /// Reads the current column value and increments the field counter.
    /// If the value is null, the passed variable is not changed.
    /// There is no straight forward way to determine, whether the value was null.
    /// You should use the get method with the null indicator, if the value
    /// might be null or just initialize your value with a suitable default.
    template <typename T>
    RowReader& get(T& ret)
    { _row[_fieldNum++].get(ret); return *this; }

    /// Reads the current column value and a null indicator and increments the
    /// field counter. If the value is null, the null indicator is set to false
    /// and the actual value of the passed variable is not changed.
    template <typename T>
    RowReader& get(T& ret, bool& nullInd)
    { nullInd = _row[_fieldNum++].get(ret); return *this; }

    /// Reads the current value into a tntdb::Value and increments the field
    /// counter.
    RowReader& get(tntdb::Value& v)
    { v = _row[_fieldNum++]; return *this; }

    /// Returns true, if the current value is null.
    bool isNull() const
    { return _row[_fieldNum].isNull(); }

    /// Resets the field counter to the passed value.
    void rewind(Row::size_type n_ = 0)
    { _fieldNum = n_; }

    /// returns the underlying _row.
    const Row& currentRow() const
    { return _row; }

    /// returns the current column number.
    Row::size_type currentCol() const
    { return _fieldNum; }

    /// increments the current column number and returns the incremented value (pre increment).
    Row::size_type operator++ ()
    { return ++_fieldNum; }

    /// increments the current column number and returns the previous value (post increment).
    Row::size_type operator++ (int)
    { return _fieldNum++; }

    /// decrements the current column number and returns the decremented value (pre decrement).
    Row::size_type operator-- ()
    { return --_fieldNum; }

    /// decrements the current column number and returns the previous value (post decrement).
    Row::size_type operator-- (int)
    { return _fieldNum--; }

};

inline RowReader Row::reader(Row::size_type n) const
{ return RowReader(*this, n); }

template <typename T>
RowReader Row::get(T& ret) const
{ return RowReader(*this).get(ret); }

template <typename T>
RowReader Row::get(T& ret, bool& nullInd) const
{ return RowReader(*this).get(ret, nullInd); }

}

#endif // TNTDB_ROWREADER_H
