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
Cursor::Cursor(Statement& statement, unsigned fetchsize)
  : _row(std::make_shared<BoundRow>(statement.getFieldCount())),
    _fields(statement.getFields()),
    _stmt(statement.createStmt())
{
    unsigned field_count = _row->getSize();

    for (unsigned n = 0; n < field_count; ++n)
    {
        if (_fields[n].length > 0x10000)
            // do not allocate buffers > 64k - use mysql_stmt_fetch_column instead later
            _fields[n].length = 0x10000;

        _row->initOutBuffer(n, _fields[n]);
    }

    log_debug("mysql_stmt_bind_result");
    if (mysql_stmt_bind_result(_stmt, _row->getMysqlBind()) != 0)
        throw MysqlStmtError("mysql_stmt_bind_result", _stmt);

    statement.execute(_stmt, fetchsize);
}

Cursor::~Cursor()
{
    if (_stmt)
    {
        log_debug("mysql_stmt_close(" << _stmt << ')');
        ::mysql_stmt_close(_stmt);
    }
}

Row Cursor::fetch()
{
    log_debug("mysql_stmt_fetch(" << _stmt << ')');
    int ret = mysql_stmt_fetch(_stmt);

    if (ret == MYSQL_DATA_TRUNCATED)
    {
        // fetch column data where truncated
        for (unsigned n = 0; n < _row->getSize(); ++n)
        {
            if (*_row->getMysqlBind()[n].length > _row->getMysqlBind()[n].buffer_length)
            {
                // actual length was longer than buffer_length, so this column is truncated
                _fields[n].length = *_row->getMysqlBind()[n].length;
                _row->initOutBuffer(n, _fields[n]);

                log_debug("mysql_stmt_fetch_column(" << _stmt << ", BIND, " << n
                    << ", 0) with " << _fields[n].length << " bytes");
                if (mysql_stmt_fetch_column(_stmt, _row->getMysqlBind() + n, n, 0) != 0)
                    throw MysqlStmtError("mysql_stmt_fetch_column", _stmt);
            }
        }
    }
    else if (ret == MYSQL_NO_DATA)
    {
        log_debug("MYSQL_NO_DATA");
        _row = 0;
        return Row();
    }
    else if (ret == 1)
        throw MysqlStmtError("mysql_stmt_fetch", _stmt);

    return Row(_row);
}
}
}
