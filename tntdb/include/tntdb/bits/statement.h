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

#ifndef TNTDB_BITS_STATEMENT_H
#define TNTDB_BITS_STATEMENT_H

#include <string>
#include <tntdb/smartptr.h>
#include <tntdb/iface/istatement.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>

namespace tntdb
{
  class Connection;
  class Result;
  class Row;
  class Value;
  class Date;
  class Time;
  class Datetime;

  /**
   * This class represents a sql-statement. A statement can have parameters,
   * which are referenced by name, called hostvariables. They are prefixed with
   * a colon followed by a name. A name starts with a letter followed by
   * alphanumeric characters or underscore. Hostvariables are not searched in
   * strings (between apostrophes, quotation marks or backticks). The backslash
   * prevents the interpretation of a special meaning of the following
   * character.
   */
  class Statement
  {
    public:
      class const_iterator;
      typedef IStatement::size_type size_type;

    private:
      SmartPtr<IStatement, InternalRefCounted> stmt;

    public:
      Statement(IStatement* stmt_ = 0)
        : stmt(stmt_)
        { }

      /**
       * Sets all hostvariables to NULL.
       */
      Statement& clear()
        { stmt->clear(); return *this; }
      /**
       * Sets the hostvariable with the given name to NULL.
       */
      Statement& setNull(const std::string& col)
        { stmt->setNull(col); return *this; }
      /**
       * Sets the hostvariable with the given name to a boolean value.
       */
      Statement& setBool(const std::string& col, bool data)
        { stmt->setBool(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a int value.
       */
      Statement& setInt(const std::string& col, int data)
        { stmt->setInt(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a unsigned value.
       */
      Statement& setUnsigned(const std::string& col, unsigned data)
        { stmt->setUnsigned(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a float value.
       */
      Statement& setFloat(const std::string& col, float data)
        { stmt->setFloat(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a double value.
       */
      Statement& setDouble(const std::string& col, double data)
        { stmt->setDouble(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a char value.
       */
      Statement& setChar(const std::string& col, char data)
        { stmt->setChar(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a string value.
       */
      Statement& setString(const std::string& col, const std::string& data)
        { stmt->setString(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a string value or null,
       * if passing null-pointer.
       */
      Statement& setString(const std::string& col, const char* data)
        { data == 0 ? stmt->setNull(col)
                    : stmt->setString(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a date value.
       */
      Statement& setDate(const std::string& col, const Date& data)
        { data.isNull() ? stmt->setNull(col)
                        : stmt->setDate(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a time value.
       */
      Statement& setTime(const std::string& col, const Time& data)
        { data.isNull() ? stmt->setNull(col)
                        : stmt->setTime(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a datetime value.
       */
      Statement& setDatetime(const std::string& col, const Datetime& data)
        { data.isNull() ? stmt->setNull(col)
                        : stmt->setDatetime(col, data); return *this; }

      //@{
      /**
       * Set the hostvariable with the given name to the passed value.
       * These set-methods choose the right bound type by parametertype.
       */
      Statement& set(const std::string& col, bool data)
        { stmt->setBool(col, data); return *this; }
      Statement& set(const std::string& col, int data)
        { stmt->setInt(col, data); return *this; }
      Statement& set(const std::string& col, unsigned data)
        { stmt->setUnsigned(col, data); return *this; }
      Statement& set(const std::string& col, float data)
        { stmt->setFloat(col, data); return *this; }
      Statement& set(const std::string& col, double data)
        { stmt->setDouble(col, data); return *this; }
      Statement& set(const std::string& col, char data)
        { stmt->setChar(col, data); return *this; }
      Statement& set(const std::string& col, const char* data)
        { data == 0 ? stmt->setNull(col)
                    : stmt->setString(col, data); return *this; }
      Statement& set(const std::string& col, const std::string& data)
        { stmt->setString(col, data); return *this; }
      Statement& set(const std::string& col, const Date& data)
        { data.isNull() ? stmt->setNull(col)
                        : stmt->setDate(col, data); return *this; }
      Statement& set(const std::string& col, const Time& data)
        { data.isNull() ? stmt->setNull(col)
                        : stmt->setTime(col, data); return *this; }
      Statement& set(const std::string& col, const Datetime& data)
        { data.isNull() ? stmt->setNull(col)
                        : stmt->setDatetime(col, data); return *this; }
      //@}

      /// statement-execution-methods
      /**
       * Executes a query with the current parameters. The query should not
       * return results. This method is normally used with INSERT-, UPDATE- or
       * DELETE-statements.
       */
      size_type execute();
      /**
       * Executes a query, which returns a resultset, with the current
       * parameters. The query is normally a SELECT-statement.
       */
      Result select();
      /**
       * Executes a query, which returns a row, with the current
       * parameters. If the query return no rows, a exception of type
       * tntdb::NotFound is thrown. When the query returns more than one row,
       * additional rows are discarded.
       */
      Row selectRow();
      /**
       * Executes a query, which returns a single value, with the current
       * parameters. If the query return no rows, a exception of type
       * tntdb::NotFound is thrown. Only the first value of the first row is
       * returned.
       */
      Value selectValue();

      /**
       * This methods creates a cursor and fetches the first row.
       */
      const_iterator begin() const;
      /**
       * A empty iterator is returned. Every const_iterator, which reaches the
       * end is equal with this.
       */
      const_iterator end() const;

      /**
       * Returns true, if this class is not connected to a actual statement.
       */
      bool operator!() const             { return !stmt; }
      /**
       * Returns the actual implementation-class.
       */
      const IStatement* getImpl() const  { return &*stmt; }
  };
}

#endif // TNTDB_BITS_STATEMENT_H

