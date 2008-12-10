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

#ifndef TNTDB_ERROR_H
#define TNTDB_ERROR_H

#include <stdexcept>
#include <string>

namespace tntdb
{
  /**
   * Baseclass for database-errors. All errors are handled by throwing this or
   * a derived class.
   */
  class Error : public std::runtime_error
  {
    public:
      /// constructor for a error-message
      explicit Error(const std::string& msg);
  };

  /**
   * Thrown, if selectRow or selectValue does not fetch any data.
   */
  class NotFound : public Error
  {
    public:
      NotFound();
  };

  /**
   * This exception is thrown, when a value is extracted with
   * Value::get...(), but the value is null.
   */
  class NullValue : public Error
  {
    public:
      NullValue();
  };

  /**
   * Exception to indicate, that a value couldn't be converted to a requested type
   */
  class TypeError : public Error
  {
    public:
      explicit TypeError(const std::string& msg = "type error");
  };

  /**
   * Exception, which is a direct result of a sql-statement
   */
  class SqlError : public Error
  {
      std::string sql;

    public:
      explicit SqlError(const std::string& sql_, const std::string& msg = "sql error");
      ~SqlError() throw()
        { }

      const std::string& getSql() const  { return sql; }
  };

  class FieldNotFound : public Error
  {
      std::string field;

    public:
      explicit FieldNotFound(const std::string& field);
      ~FieldNotFound() throw() { }

      const std::string& getField() const   { return field; }
  };
}

#endif // TNTDB_ERROR_H

