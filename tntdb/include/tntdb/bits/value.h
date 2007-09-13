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

#ifndef TNTDB_BITS_VALUE_H
#define TNTDB_BITS_VALUE_H

#include <tntdb/iface/ivalue.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/smartptr.h>

namespace tntdb
{
  /**
   * The class Value represents a value, which is fetched from the database.
   */
  class Value
  {
	public:
	  typedef unsigned size_type;

	private:
	  SmartPtr<IValue, InternalRefCounted> value;

	public:
	  explicit Value(IValue* value = 0)
		: value(value)
		{ }

      //@{
      /**
       * Explicit data-access.
       * The get-methods return the value in the requested type.
       * If the value can't be converted to the requested type, a exception of
       * type tntdb::TypeError is thrown.
       */
      /// return true, when value is null
	  bool isNull() const                 { return !value || value->isNull(); }
      /// return true, when value represents boolean true.
	  bool getBool() const                { return value->getBool(); }
      /// tries to convert value into an int.
	  int getInt() const                  { return value->getInt(); }
      /// tries to convert value into an unsigned.
	  unsigned getUnsigned() const        { return value->getUnsigned(); }
      /// tries to convert value into an float.
	  float getFloat() const              { return value->getFloat(); }
      /// tries to convert value into an double.
	  double getDouble() const            { return value->getDouble(); }
      /// returns the first character of the texte-representation.
	  char getChar() const                { return value->getChar(); }
      /// returns the value as a string.
	  std::string getString() const       { std::string ret; value->getString(ret); return ret; }
      /// returns the value as a Date.
      Date getDate() const                { return value->getDate(); }
      /// returns the value as a Time.
      Time getTime() const                { return value->getTime(); }
      /// returns the value as a Datetime.
      Datetime getDatetime() const        { return value->getDatetime(); }
      //@}

      //@{
      /**
       * Make Value-objects implicit convertable.
       * This class defines implicit convert-operators to convert the value
       * automatically to different types.
       * They are identical with the corresponding get-methods.
       */
      operator bool() const               { return value->getBool(); }
      operator int() const                { return value->getInt(); }
      operator unsigned() const           { return value->getUnsigned(); }
      operator float() const              { return value->getFloat(); }
      operator double() const             { return value->getDouble(); }
      operator char() const               { return value->getChar(); }
      operator std::string() const        { std::string ret; value->getString(ret); return ret; }
      operator Date() const               { return value->getDate(); }
      operator Time() const               { return value->getTime(); }
      operator Datetime() const           { return value->getDatetime(); }
      //@}

      /// Returns true, if this class is not connected to a actual statement.
      bool operator!() const              { return !value; }
      ///  Returns the actual implementation-class.
      const IValue* getImpl() const       { return &*value; }
  };
}

#endif // TNTDB_BITS_VALUE_H

