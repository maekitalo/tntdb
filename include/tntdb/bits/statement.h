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
#include <cxxtools/convert.h>
#include <tntdb/config.h>
#include <vector>
#include <list>
#include <deque>
#include <set>

#if __cplusplus >= 201103L

#include <forward_list>
#include <unordered_set>
#include <unordered_map>

#endif


namespace tntdb
{
  class Connection;
  class Result;
  class Row;
  class Value;
  class Date;
  class Time;
  class Datetime;

  /** This class represents an SQL statement

      A statement can have parameters, which are referenced by name, called host variables.
      They are prefixed with a colon followed by a name. A name starts with a letter
      followed by alphanumeric characters or underscores. Host variables are not searched
      in strings (between apostrophes, quotation marks or backticks). A backslash prevents
      the interpretation of a special meaning of the following character.
   */
  class Statement
  {
    public:
      class const_iterator;
      typedef IStatement::size_type size_type;

    private:
      cxxtools::SmartPtr<IStatement> _stmt;

    public:
      Statement(IStatement* stmt = 0)
        : _stmt(stmt)
        { }

      /// Set all host variables to NULL
      Statement& clear()
        { _stmt->clear(); return *this; }

      /// Set the host variable with the given name to NULL
      Statement& setNull(const std::string& col)
        { _stmt->setNull(col); return *this; }

      /// Set the host variable with the given name to a boolean value
      Statement& setBool(const std::string& col, bool data)
        { _stmt->setBool(col, data); return *this; }

      /// Set the host variable with the given name to a short value
      Statement& setShort(const std::string& col, short data)
        { _stmt->setShort(col, data); return *this; }

      /// Set the host variable with the given name to an int value
      Statement& setInt(const std::string& col, int data)
        { _stmt->setInt(col, data); return *this; }

      /// Set the host variable with the given name to a long value
      Statement& setLong(const std::string& col, long data)
        { _stmt->setLong(col, data); return *this; }

      /// Set the host variable with the given name to an unsigned short value
      Statement& setUnsignedShort(const std::string& col, unsigned short data)
        { _stmt->setUnsignedShort(col, data); return *this; }

      /// Set the host variable with the given name to an unsigned value
      Statement& setUnsigned(const std::string& col, unsigned data)
        { _stmt->setUnsigned(col, data); return *this; }

      /// Set the host variable with the given name to an unsigned long value
      Statement& setUnsignedLong(const std::string& col, unsigned long data)
        { _stmt->setUnsignedLong(col, data); return *this; }

      /// Set the host variable with the given name to a int32_t value
      Statement& setInt32(const std::string& col, int32_t data)
        { _stmt->setInt32(col, data); return *this; }

      /// Set the host variable with the given name to a uint32_t value
      Statement& setUnsigned32(const std::string& col, uint32_t data)
        { _stmt->setUnsigned32(col, data); return *this; }

      /// Set the host variable with the given name to a int64_t value
      Statement& setInt64(const std::string& col, int64_t data)
        { _stmt->setInt64(col, data); return *this; }

      /// Set the host variable with the given name to a uint64_t value
      Statement& setUnsigned64(const std::string& col, uint64_t data)
        { _stmt->setUnsigned64(col, data); return *this; }

      /// Set the host variable with the given name to a Decimal
      Statement& setDecimal(const std::string& col, const Decimal& data)
        { _stmt->setDecimal(col, data); return *this; }

      /// Set the host variable with the given name to a float value
      Statement& setFloat(const std::string& col, float data)
        { _stmt->setFloat(col, data); return *this; }

      /// Set the host variable with the given name to a double value
      Statement& setDouble(const std::string& col, double data)
        { _stmt->setDouble(col, data); return *this; }

      /// Set the host variable with the given name to a char value
      Statement& setChar(const std::string& col, char data)
        { _stmt->setChar(col, data); return *this; }

      /// Set the host variable with the given name to a string value
      Statement& setString(const std::string& col, const std::string& data)
        { _stmt->setString(col, data); return *this; }

      /// Set the host variable with the given name to a string value or null
      Statement& setString(const std::string& col, const char* data)
        { data == 0 ? _stmt->setNull(col)
                    : _stmt->setString(col, data); return *this; }

      /// Set the host variable with the given name to a unicode string value
      Statement& setUString(const std::string& col, const cxxtools::String& data)
        { _stmt->setUString(col, data); return *this; }

      /// Set the host variable with the given name to a blob value
      Statement& setBlob(const std::string& col, const Blob& data)
        { _stmt->setBlob(col, data); return *this; }

      /// Set the host variable with the given name to a date value
      Statement& setDate(const std::string& col, const Date& data)
        { data.isNull() ? _stmt->setNull(col)
                        : _stmt->setDate(col, data); return *this; }

      /// Set the host variable with the given name to a time value
      Statement& setTime(const std::string& col, const Time& data)
        { data.isNull() ? _stmt->setNull(col)
                        : _stmt->setTime(col, data); return *this; }

      /// Set the host variable with the given name to a datetime value
      Statement& setDatetime(const std::string& col, const Datetime& data)
        { data.isNull() ? _stmt->setNull(col)
                        : _stmt->setDatetime(col, data); return *this; }

      /** Set the host variable with the given name to the passed value

          The method uses the operator<< with a l-value of the type Hostvar& and r-value
          of a const reference to the actual type to read the value. The operator
          is defined for standard types and may be defined for user defined types.
       */
      template <typename T>
      Statement& set(const std::string& col, const T& data);

      /** Set multiple numbered parameters to the values specified by the iterator range

          The method expects, that the statement has columns with the specified column name
          appended by a number range starting from 0. This list can be generated for example
          with tntdb::SqlBuilder.

          Example:

          @code
            std::vector<int> values;
            values.push_back(5);
            values.push_back(12);
            tntdb::Statement stmt = conn.prepare("select a, b, c from tab1 where v in (" + tntdb::Statement::paramlist("v", values.size()) + ")");
            stmt.set("v", values.begin(), values.end());
            // or short version:
            stmt.set("v", values);

            // now stmt is ready for iteration
          @endcode
       */
      template <typename Iterator>
      Statement& set(const std::string& col, Iterator it1, Iterator it2);

      /** Set the host variable with the given name to the passed value or null

          The method sets the host variable to the given value if the 2nd
          argument is true. Otherwise the host variable is set to null.
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

      /// Statement execution methods
      /// @{
      /** Execute the query without returning the result

          The query should not return results. This method is normally used with
          INSERT, UPDATE or DELETE statements.
       */
      size_type execute();

      /** Execute the query and return the result

          The query normally is a SELECT statement.
       */
      Result select();

      /** Execute the query and return the first row of the result

          If the result is empty, a NotFound exception is thrown. Additional rows are ignored.
       */
      Row selectRow();
      /** Execute the query and return the first value from the first row of the result

          If the result is empty, a NotFound exception is thrown. Additional values are ignored.
       */
      Value selectValue();
      /// @}

      /// Create a database cursor and fetch the first row of the query result
      const_iterator begin(unsigned fetchsize = 100) const;

      /** Get an end iterator

          This iterator works like the iterator got from the %end() method of STL containers
          like std::vector and std::list. It doesn't reference the last element in a list but
          an imaginary element after the last one, and should be used as follows:

          @code
            for(tntdb::Statement::const_iterator it = stmtObj.begin(); it != stmtObj.end(); ++it)
            {
              // Loop body accessing individual result rows through *it
            }
          @endcode
       */
      const_iterator end() const;

      /// Check whether this object is associated with a real statement (<b>true if not</b>)
      bool operator!() const            { return !_stmt; }

      /** Sets a buffer for delayed execution of statements.

          When the number if greater than 0 and the connection is in a
          transaction, the execute method may delay the actual execution until
          the buffer is full or flush is called.

          The default is 0.
       */
      void maxNumDelay(unsigned n);
      /// Returns the number of the currently pending statements.
      unsigned numDelayed();
      /// Executes all currently pending statements.
      void flush();

      /// @{
      /// Get the actual implementation object
      const IStatement* getImpl() const { return &*_stmt; }
      IStatement* getImpl()             { return &*_stmt; }
      /// @}
  };

  /// Helper class to hold a Statement and a column name
  class Hostvar
  {
    private:
      Statement& _stmt;
      const std::string& _name;

    public:
      Hostvar(Statement& stmt, const std::string& name)
        : _stmt(stmt),
          _name(name)
          { }
      Statement& getStatement()    { return _stmt; }
      const std::string& getName() { return _name; }

      void setNull()
        { _stmt.setNull(_name); }
      void setBool(bool data)
        { _stmt.setBool(_name, data); }
      void setInt(int data)
        { _stmt.setInt(_name, data); }
      void setLong(long data)
        { _stmt.setLong(_name, data); }
      void setUnsigned(unsigned data)
        { _stmt.setUnsigned(_name, data); }
      void setUnsignedLong(unsigned long data)
        { _stmt.setUnsignedLong(_name, data); }
      void setInt32(int32_t data)
        { _stmt.setInt32(_name, data); }
      void setUnsigned32(uint32_t data)
        { _stmt.setUnsigned32(_name, data); }
      void setInt64(int64_t data)
        { _stmt.setInt64(_name, data); }
      void setUnsigned64(uint64_t data)
        { _stmt.setUnsigned64(_name, data); }
      void setDecimal(const Decimal& data)
        { _stmt.setDecimal(_name, data); }
      void setFloat(float data)
        { _stmt.setFloat(_name, data); }
      void setDouble(double data)
        { _stmt.setDouble(_name, data); }
      void setChar(char data)
        { _stmt.setChar(_name, data); }
      void setString(const std::string& data)
        { _stmt.setString(_name, data); }
      void setString(const char* data)
        { data == 0 ? _stmt.setNull(_name)
                    : _stmt.setString(_name, data); }
      void setUString(const cxxtools::String& data)
        { _stmt.setUString(_name, data); }
      void setBlob(const Blob& data)
        { _stmt.setBlob(_name, data); }
      void setDate(const Date& data)
        { data.isNull() ? _stmt.setNull(_name)
                        : _stmt.setDate(_name, data); }
      void setTime(const Time& data)
        { data.isNull() ? _stmt.setNull(_name)
                        : _stmt.setTime(_name, data); }
      void setDatetime(const Datetime& data)
        { data.isNull() ? _stmt.setNull(_name)
                        : _stmt.setDatetime(_name, data); }

      template <typename T>
      void set(const T& data);
  };

  /// Set operators for host variables
  /// @{

  inline void operator<< (Hostvar& hostvar, bool data)
    { hostvar.setBool(data); }

  inline void operator<< (Hostvar& hostvar, int data)
    { hostvar.setInt(data); }

  inline void operator<< (Hostvar& hostvar, long data)
    { hostvar.setLong(data); }

  inline void operator<< (Hostvar& hostvar, unsigned data)
    { hostvar.setUnsigned(data); }

  inline void operator<< (Hostvar& hostvar, unsigned long data)
    { hostvar.setUnsignedLong(data); }

#if INT_INT32_T_CONFLICT != 1
  inline void operator<< (Hostvar& hostvar, int32_t data)
    { hostvar.setInt32(data); }
#endif

#if UNSIGNED_UINT32_T_CONFLICT != 1
  inline void operator<< (Hostvar& hostvar, uint32_t data)
    { hostvar.setUnsigned32(data); }
#endif

#if INT_INT64_T_CONFLICT != 1
  inline void operator<< (Hostvar& hostvar, int64_t data)
    { hostvar.setInt64(data); }
#endif

#if UNSIGNED_UINT64_T_CONFLICT != 1
  inline void operator<< (Hostvar& hostvar, uint64_t data)
    { hostvar.setUnsigned64(data); }
#endif

  inline void operator<< (Hostvar& hostvar, const Decimal& data)
    { hostvar.setDecimal(data); }

  inline void operator<< (Hostvar& hostvar, float data)
    { hostvar.setFloat(data); }

  inline void operator<< (Hostvar& hostvar, double data)
    { hostvar.setDouble(data); }

  inline void operator<< (Hostvar& hostvar, char data)
    { hostvar.setChar(data); }

  inline void operator<< (Hostvar& hostvar, const std::string& data)
    { hostvar.setString(data); }

  inline void operator<< (Hostvar& hostvar, const char* data)
    { hostvar.setString(data); }

  inline void operator<< (Hostvar& hostvar, const cxxtools::String& data)
    { hostvar.setUString(data); }

  inline void operator<< (Hostvar& hostvar, const Blob& data)
    { hostvar.setBlob(data); }

  inline void operator<< (Hostvar& hostvar, const Date& data)
    { hostvar.setDate(data); }

  inline void operator<< (Hostvar& hostvar, const Time& data)
    { hostvar.setTime(data); }

  inline void operator<< (Hostvar& hostvar, const Datetime& data)
    { hostvar.setDatetime(data); }

  template <typename T>
  void operator<< (Hostvar& hostvar, const std::vector<T>& data)
    { hostvar.getStatement().set(hostvar.getName(), data.begin(), data.end()); }

  template <typename T>
  void operator<< (Hostvar& hostvar, const std::list<T>& data)
    { hostvar.getStatement().set(hostvar.getName(), data.begin(), data.end()); }

  template <typename T>
  void operator<< (Hostvar& hostvar, const std::deque<T>& data)
    { hostvar.getStatement().set(hostvar.getName(), data.begin(), data.end()); }

  template <typename T>
  void operator<< (Hostvar& hostvar, const std::set<T>& data)
    { hostvar.getStatement().set(hostvar.getName(), data.begin(), data.end()); }

  template <typename T>
  void operator<< (Hostvar& hostvar, const std::multiset<T>& data)
    { hostvar.getStatement().set(hostvar.getName(), data.begin(), data.end()); }

#if __cplusplus >= 201103L

  template <typename T>
  void operator<< (Hostvar& hostvar, const std::forward_list<T>& data)
    { hostvar.getStatement().set(hostvar.getName(), data.begin(), data.end()); }

  template <typename T>
  void operator<< (Hostvar& hostvar, const std::unordered_set<T>& data)
    { hostvar.getStatement().set(hostvar.getName(), data.begin(), data.end()); }

  template <typename T>
  void operator<< (Hostvar& hostvar, const std::unordered_multiset<T>& data)
    { hostvar.getStatement().set(hostvar.getName(), data.begin(), data.end()); }

#endif

  /// @}

  template <typename T>
  Statement& Statement::set(const std::string& name, const T& data)
  {
    Hostvar h(*this, name);
    h << data;
    return *this;
  }

  template <typename Iterator>
  Statement& Statement::set(const std::string& col, Iterator it1, Iterator it2)
  {
    for (unsigned n = 0; it1 != it2; ++n, ++it1)
    {
      set(col + cxxtools::convert<std::string>(n), *it1);
    }
    return *this;
  }

  template <typename T>
  void Hostvar::set(const T& data)
    { *this << data; }
}

#endif // TNTDB_BITS_STATEMENT_H

