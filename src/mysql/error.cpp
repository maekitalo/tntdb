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

#include <tntdb/mysql/error.h>
#include <sstream>

namespace tntdb
{
  namespace mysql
  {
    namespace
    {
      std::string errorMessage(const char* function, MYSQL* mysql)
      {
        std::ostringstream msg;
        msg << "Mysql-Error " << mysql_errno(mysql)
            << " in " << function << ": "
            << ::mysql_error(mysql);
        return msg.str();
      }

      std::string errorMessage(MYSQL* mysql)
      {
        std::ostringstream msg;
        msg << "Mysql-Error " << mysql_errno(mysql) << ": "
            << ::mysql_error(mysql);
        return msg.str();
      }

      std::string errorMessage(const char* function, MYSQL_STMT* stmt)
      {
        std::ostringstream msg;
        msg << "Mysql-Error " << mysql_stmt_errno(stmt)
            << " in " << function << ": "
            << ::mysql_stmt_error(stmt);
        return msg.str();
      }

      std::string errorMessage(MYSQL_STMT* stmt)
      {
        std::ostringstream msg;
        msg << "Mysql-Error " << mysql_stmt_errno(stmt) << ": "
            << ::mysql_stmt_error(stmt);
        return msg.str();
      }
    }

    MysqlError::MysqlError(MYSQL* mysql)
      : Error(errorMessage(mysql))
      { }

    MysqlError::MysqlError(const char* function, MYSQL* mysql)
      : Error(errorMessage(function, mysql))
      { }

    MysqlStmtError::MysqlStmtError(MYSQL_STMT* stmt)
      : MysqlError(errorMessage(stmt))
      { }

    MysqlStmtError::MysqlStmtError(const char* function,
      MYSQL_STMT* stmt)
      : MysqlError(errorMessage(function, stmt))
      { }
  }
}
