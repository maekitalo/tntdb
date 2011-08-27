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

#ifndef TNTDB_BITS_ROW_H
#define TNTDB_BITS_ROW_H

#include <tntdb/iface/irow.h>
#include <tntdb/bits/value.h>
#include <tntdb/blob.h>
#include <tntdb/config.h>
#include <cxxtools/smartptr.h>

namespace tntdb
{
  class RowReader;

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
      cxxtools::SmartPtr<IRow> row;

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
        { return row->getValueByNumber(field_num); }
      /// Returns the value-object by name.
      /// Note that this is less effective than fetching by number
      Value getValue(const std::string& field_name) const
        { return row->getValueByName(field_name); }
      /// Returns the field_num's value-object.
      Value operator[] (size_type field_num) const
        { return row->getValueByNumber(field_num); }
      /// Returns the value-object by name.
      /// Note that this is less effective than fetching by number
      Value operator[] (const std::string& field_name) const
        { return row->getValueByName(field_name); }

      /// Return true, if the specified value is null.
      bool isNull(size_type field_num) const
        { return getValue(field_num).isNull(); }
      /// Return true, if the specified value is null.
      bool isNull(const std::string& field_name) const
        { return getValue(field_name).isNull(); }

      //@{
      /**
       * The get-methods try to convert the value of the speicfied column to
       * the requested type.
       */

      bool getBool(size_type field_num) const
        { return getValue(field_num).getBool(); }
      short getShort(size_type field_num) const
        { return getValue(field_num).getShort(); }
      int getInt(size_type field_num) const
        { return getValue(field_num).getInt(); }
      long getLong(size_type field_num) const
        { return getValue(field_num).getLong(); }
      unsigned short getUnsignedShort(size_type field_num) const
        { return getValue(field_num).getUnsignedShort(); }
      unsigned getUnsigned(size_type field_num) const
        { return getValue(field_num).getUnsigned(); }
      unsigned long getUnsignedLong(size_type field_num) const
        { return getValue(field_num).getUnsignedLong(); }
      int32_t getInt32(size_type field_num) const
        { return getValue(field_num).getInt32(); }
      uint32_t getUnsigned32(size_type field_num) const
        { return getValue(field_num).getUnsigned32(); }
      int64_t getInt64(size_type field_num) const
        { return getValue(field_num).getInt64(); }
      uint64_t getUnsigned64(size_type field_num) const
        { return getValue(field_num).getUnsigned64(); }
      Decimal getDecimal(size_type field_num) const
        { return getValue(field_num).getDecimal(); }
      float getFloat(size_type field_num) const
        { return getValue(field_num).getFloat(); }
      double getDouble(size_type field_num) const
        { return getValue(field_num).getDouble(); }
      char getChar(size_type field_num) const
        { return getValue(field_num).getChar(); }
      std::string getString(size_type field_num) const
        { return getValue(field_num).getString(); }
      void getString(size_type field_num, std::string& ret) const
        { return getValue(field_num).getString(ret); }
      Blob getBlob(size_type field_num) const
        { return getValue(field_num).getBlob(); }
      void getBlob(size_type field_num, Blob& ret) const
        { return getValue(field_num).getBlob(ret); }
      Date getDate(size_type field_num) const
        { return getValue(field_num).getDate(); }
      Time getTime(size_type field_num) const
        { return getValue(field_num).getTime(); }
      Datetime getDatetime(size_type field_num) const
        { return getValue(field_num).getDatetime(); }

      bool getBool(const std::string& field_name) const
        { return getValue(field_name).getBool(); }
      int getInt(const std::string& field_name) const
        { return getValue(field_name).getInt(); }
      long getLong(const std::string& field_name) const
        { return getValue(field_name).getLong(); }
      unsigned getUnsigned(const std::string& field_name) const
        { return getValue(field_name).getUnsigned(); }
      unsigned long getUnsignedLong(const std::string& field_name) const
        { return getValue(field_name).getUnsignedLong(); }
      int32_t getInt32(const std::string& field_name) const
        { return getValue(field_name).getInt32(); }
      uint32_t getUnsigned32(const std::string& field_name) const
        { return getValue(field_name).getUnsigned32(); }
      int64_t getInt64(const std::string& field_name) const
        { return getValue(field_name).getInt64(); }
      uint64_t getUnsigned64(const std::string& field_name) const
        { return getValue(field_name).getUnsigned64(); }
      Decimal getDecimal(const std::string& field_name) const
        { return getValue(field_name).getDecimal(); }
      float getFloat(const std::string& field_name) const
        { return getValue(field_name).getFloat(); }
      double getDouble(const std::string& field_name) const
        { return getValue(field_name).getDouble(); }
      char getChar(const std::string& field_name) const
        { return getValue(field_name).getChar(); }
      std::string getString(const std::string& field_name) const
        { return getValue(field_name).getString(); }
      void getString(const std::string& field_name, std::string& ret) const
        { return getValue(field_name).getString(ret); }
      Blob getBlob(const std::string& field_name) const
        { return getValue(field_name).getBlob(); }
      void getBlob(const std::string& field_name, Blob& ret) const
        { return getValue(field_name).getBlob(ret); }
      Date getDate(const std::string& field_name) const
        { return getValue(field_name).getDate(); }
      Time getTime(const std::string& field_name) const
        { return getValue(field_name).getTime(); }
      Datetime getDatetime(const std::string& field_name) const
        { return getValue(field_name).getDatetime(); }
      //@}

      template <typename T>
      bool getValue(size_type field_num, T& ret) const
        { return getValue(field_num).getValue(ret); }

      /// Returns a iterator to the first column
      const_iterator begin() const;
      /// Returns a iterator past the last column
      const_iterator end() const;

      /// Returns true, if this class is not connected to a actual databaserow.
      bool operator!() const         { return !row; }
      /// Returns the actual implementation-class.
      const IRow* getImpl() const    { return &*row; }

      /// Instantiates a tntdb::RowReader with the passed initial column counter.
      RowReader reader(size_type n = 0) const;
  };
}

#endif // TNTDB_BITS_ROW_H

