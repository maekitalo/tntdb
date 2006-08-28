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

#include <tntdb/mysql/impl/cursor.h>
#include <tntdb/mysql/impl/statement.h>
#include <tntdb/mysql/impl/boundrow.h>
#include <tntdb/row.h>
#include <tntdb/mysql/error.h>
#include <mysql.h>
#include <cxxtools/log.h>

log_define("tntdb.mysql.cursor")

namespace tntdb
{
  namespace mysql
  {
    Cursor::Cursor(Statement* statement)
      : row(new BoundRow(statement->getFieldCount())),
        mysqlStatement(statement),
        stmt(statement->getStmt())
    {
      MYSQL_FIELD* fields = statement->getFields();
      unsigned field_count = statement->getFieldCount();

      for (unsigned n = 0; n < field_count; ++n)
        row->initOutBuffer(n, fields[n]);

      log_debug("mysql_stmt_bind_result");
      if (mysql_stmt_bind_result(stmt, row->getMysqlBind()) != 0)
        throw MysqlStmtError("mysql_stmt_bind_result", stmt);

      statement->execute(stmt);
    }

    Cursor::~Cursor()
    {
      if (stmt)
        mysqlStatement->putback(stmt);
    }

    Row Cursor::fetch()
    {
      log_debug("mysql_stmt_fetch(" << stmt << ')');
      int ret = mysql_stmt_fetch(stmt);

      if (ret == MYSQL_NO_DATA)
      {
        log_debug("MYSQL_NO_DATA");
        row = 0;
        return Row();
      }
      else if (ret == 1)
        throw MysqlStmtError("mysql_stmt_fetch", stmt);

      return Row(&*row);
    }
  }
}
