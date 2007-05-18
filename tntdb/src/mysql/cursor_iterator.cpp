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

#include <tntdb/mysql/cursor.h>
#include <tntdb/mysql/error.h>
#include <cxxtools/log.h>
#include <mysql.h>

log_define("tntdb.mysql.cursor.iterator")

namespace tntdb
{
  namespace mysql
  {
    Cursor::const_iterator::const_iterator(const Cursor& cursor_)
      : cursor(cursor_)
    {
      // if not already done
      //   parse hostvars
      //   mysql_stmt_prepare
      //   mysql_stmt_result_metadata
      //   allocate parameter
      //   mysql_stmt_bind_parm
      //   allocate result
      //   mysql_stmt_bind_result

      // mysql_stmt_execute
      // fetch first row
    }

    Cursor::const_iterator& Cursor::const_iterator::operator++()
    {
      // fetch next row
      log_debug("mysql_stmt_fetch");
      int ret = ::mysql_stmt_fetch(getStmt());

      if (ret == 1)
        throw Error("mysql_stmt_fetch", getDb());
      else if (ret == MYSQL_NO_DATA)
        cursor = Cursor();
#ifdef MYSQL_DATA_TRUNCATED
      else if (ret == MYSQL_DATA_TRUNCATED)
      {
        log_error("mysql_stmt_fetch returned MYSQL_DATA_TRUNCATED");
        throw std::runtime_error("mysql_stmt_fetch returned MYSQL_DATA_TRUNCATED");
      }
#endif
    }
  }
}
