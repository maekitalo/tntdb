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

#ifndef TNTDB_ERROR_H
#define TNTDB_ERROR_H

#include <stdexcept>

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
      NotFound()
        : Error("not found")
        { }
  };

  /**
   * This exception is thrown, when a value is extracted with
   * Value::get...(), but the value is null.
   */
  class NullValue : public Error
  {
    public:
      NullValue()
        : Error("null value")
        { }
  };

  /**
   * Exception to indicate, that a value couldn't be converted to a requested type
   */
  class TypeError : public Error
  {
    public:
      explicit TypeError(const std::string& msg = "type error")
        : Error(msg)
        { }
  };

  /**
   * Exception to indicate, that a value couldn't be converted to a requested type
   */
  class SqlError : public Error
  {
      std::string sql;

    public:
      explicit SqlError(const std::string& sql_, const std::string& msg = "sql error")
        : Error(msg),
          sql(sql_)
        { }
      ~SqlError() throw()
        { }

      const std::string& getSql() const  { return sql; }
  };
}

#endif // TNTDB_ERROR_H

