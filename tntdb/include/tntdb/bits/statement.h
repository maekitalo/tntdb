/*
 * Copyright (C) 2005,2010 Tommi Maekitalo
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

#ifndef TNTDB_BITS_STATEMENT_H
#define TNTDB_BITS_STATEMENT_H

#include <string>
#include <cxxtools/smartptr.h>
#include <tntdb/iface/istatement.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/config.h>

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
      cxxtools::SmartPtr<IStatement> stmt;

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
       * Sets the hostvariable with the given name to a short value.
       */
      Statement& setShort(const std::string& col, short data)
        { stmt->setShort(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a int value.
       */
      Statement& setInt(const std::string& col, int data)
        { stmt->setInt(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a int value.
       */
      Statement& setLong(const std::string& col, long data)
        { stmt->setLong(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a unsigned short value.
       */
      Statement& setUnsignedShort(const std::string& col, unsigned short data)
        { stmt->setUnsignedShort(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a unsigned value.
       */
      Statement& setUnsigned(const std::string& col, unsigned data)
        { stmt->setUnsigned(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a unsigned long value.
       */
      Statement& setUnsignedLong(const std::string& col, unsigned long data)
        { stmt->setUnsignedLong(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a int32_t value.
       */
      Statement& setInt32(const std::string& col, int32_t data)
        { stmt->setInt32(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a uint32_t value.
       */
      Statement& setUnsigned32(const std::string& col, uint32_t data)
        { stmt->setUnsigned32(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a int64_t value.
       */
      Statement& setInt64(const std::string& col, int64_t data)
        { stmt->setInt64(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a uint64_t value.
       */
      Statement& setUnsigned64(const std::string& col, uint64_t data)
        { stmt->setUnsigned64(col, data); return *this; }
      /**
       * Sets the hostvariable with the given name to a Decimal.
       */
      Statement& setDecimal(const std::string& col, const Decimal& data)
        { stmt->setDecimal(col, data); return *this; }
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
      Statement& setBlob(const std::string& col, const Blob& data)
        { stmt->setBlob(col, data); return *this; }
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
       *
       * The method uses the operator<< with a l-value of tntdb::Hostvar&
       * and r-value of a const reference to the actual type to read the
       * value. The operator is defined for standard types and may be defined
       * for user defined types.
       */
      template <typename T>
      Statement& set(const std::string& col, const T& data);
      //@}

      /**
       * Set the hostvariable with the given name to the passed value or null.
       *
       * The method sets the hostvariable to the given value if the 2nd
       * argument is true. Otherwise the hostvariable is set to null.
       */
      template <typename T>
      Statement& setIf(const std::string& col, bool notNull, const T& data)
      {
        if (notNull)
          set(col, data);
        else
          setNull(col);
        return *this;
      }

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
      const_iterator begin(unsigned fetchsize = 100) const;
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
      IStatement* getImpl()              { return &*stmt; }
  };

  /**
   * Helper class to hold a tntdb::Statement and a column name.
   */
  class Hostvar
  {
      Statement& stmt;
      const std::string& name;

    public:
      Hostvar(Statement& stmt_, const std::string& name_)
        : stmt(stmt_),
          name(name_)
          { }
      Statement& getStatement()   { return stmt; }
      const std::string& getName() { return name; }

      void setNull()
        { stmt.setNull(name); }
      void setBool(bool data)
        { stmt.setBool(name, data); }
      void setInt(int data)
        { stmt.setInt(name, data); }
      void setLong(long data)
        { stmt.setLong(name, data); }
      void setUnsigned(unsigned data)
        { stmt.setUnsigned(name, data); }
      void setUnsignedLong(unsigned long data)
        { stmt.setUnsignedLong(name, data); }
      void setInt32(int32_t data)
        { stmt.setInt32(name, data); }
      void setUnsigned32(uint32_t data)
        { stmt.setUnsigned32(name, data); }
      void setInt64(int64_t data)
        { stmt.setInt64(name, data); }
      void setUnsigned64(uint64_t data)
        { stmt.setUnsigned64(name, data); }
      void setDecimal(const Decimal& data)
        { stmt.setDecimal(name, data); }
      void setFloat(float data)
        { stmt.setFloat(name, data); }
      void setDouble(double data)
        { stmt.setDouble(name, data); }
      void setChar(char data)
        { stmt.setChar(name, data); }
      void setString(const std::string& data)
        { stmt.setString(name, data); }
      void setString(const char* data)
        { data == 0 ? stmt.setNull(name)
                    : stmt.setString(name, data); }
      void setBlob(const Blob& data)
        { stmt.setBlob(name, data); }
      void setDate(const Date& data)
        { data.isNull() ? stmt.setNull(name)
                        : stmt.setDate(name, data); }
      void setTime(const Time& data)
        { data.isNull() ? stmt.setNull(name)
                        : stmt.setTime(name, data); }
      void setDatetime(const Datetime& data)
        { data.isNull() ? stmt.setNull(name)
                        : stmt.setDatetime(name, data); }

      template <typename T>
      void set(const T& data);
  };

  //@{
  /**
   * Set operators for host variables.
   */
  inline void operator<< (Hostvar& hostvar, bool data)
  {
    hostvar.setBool(data);
  }

  inline void operator<< (Hostvar& hostvar, int data)
  {
    hostvar.setInt(data);
  }

  inline void operator<< (Hostvar& hostvar, long data)
  {
    hostvar.setLong(data);
  }

  inline void operator<< (Hostvar& hostvar, unsigned data)
  {
    hostvar.setUnsigned(data);
  }

  inline void operator<< (Hostvar& hostvar, unsigned long data)
  {
    hostvar.setUnsignedLong(data);
  }

#if INT_INT32_T_CONFLICT != 1
  inline void operator<< (Hostvar& hostvar, int32_t data)
  {
    hostvar.setInt32(data);
  }
#endif

#if UNSIGNED_UINT32_T_CONFLICT != 1
  inline void operator<< (Hostvar& hostvar, uint32_t data)
  {
    hostvar.setUnsigned32(data);
  }
#endif

#if INT_INT64_T_CONFLICT != 1
  inline void operator<< (Hostvar& hostvar, int64_t data)
  {
    hostvar.setInt64(data);
  }
#endif

#if UNSIGNED_UINT64_T_CONFLICT != 1
  inline void operator<< (Hostvar& hostvar, uint64_t data)
  {
    hostvar.setUnsigned64(data);
  }
#endif

  // TODO
  inline void operator<< (Hostvar& hostvar, const Decimal& data)
  {
    hostvar.setDecimal(data);
  }

  inline void operator<< (Hostvar& hostvar, float data)
  {
    hostvar.setFloat(data);
  }

  inline void operator<< (Hostvar& hostvar, double data)
  {
    hostvar.setDouble(data);
  }

  inline void operator<< (Hostvar& hostvar, char data)
  {
    hostvar.setChar(data);
  }

  inline void operator<< (Hostvar& hostvar, const std::string& data)
  {
    hostvar.setString(data);
  }

  inline void operator<< (Hostvar& hostvar, const char* data)
  {
    hostvar.setString(data);
  }

  inline void operator<< (Hostvar& hostvar, const Blob& data)
  {
    hostvar.setBlob(data);
  }

  inline void operator<< (Hostvar& hostvar, const Date& data)
  {
    hostvar.setDate(data);
  }

  inline void operator<< (Hostvar& hostvar, const Time& data)
  {
    hostvar.setTime(data);
  }

  inline void operator<< (Hostvar& hostvar, const Datetime& data)
  {
    hostvar.setDatetime(data);
  }
  //@}

  template <typename T>
  Statement& Statement::set(const std::string& name, const T& data)
  {
    Hostvar h(*this, name);
    h << data;
    return *this;
  }

  template <typename T>
  void Hostvar::set(const T& data)
  {
    *this << data;
  }

}

#endif // TNTDB_BITS_STATEMENT_H

