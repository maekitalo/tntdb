/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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

