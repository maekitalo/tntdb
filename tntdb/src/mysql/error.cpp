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
