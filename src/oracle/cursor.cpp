/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#include <tntdb/oracle/cursor.h>
#include <tntdb/oracle/row.h>
#include <tntdb/oracle/singlerow.h>
#include <tntdb/oracle/statement.h>
#include <cxxtools/log.h>

log_define("tntdb.oracle.cursor")

namespace tntdb
{
namespace oracle
{
Cursor::Cursor(Statement& stmt, unsigned fetchsize)
  : _conn(stmt.getConnection()),
    _fetchsize(fetchsize),
    _row(0),
    _rowcount(0)
{
    log_debug("execute statement " << stmt.getHandle());
    sword ret = OCIStmtExecute(_conn.getSvcCtxHandle(),
        stmt.getHandle(), _conn.getErrorHandle(), 0, 0, 0, 0, OCI_DEFAULT);
    _conn.checkError(ret, "OCIStmtExecute");

    auto mr = std::make_shared<MultiRow>(stmt, _fetchsize);
    _row = std::make_shared<SingleRow>(mr, 0);

    _stmtp = stmt.getHandle();
}

tntdb::Row Cursor::fetch()
{
    if (_rowcount > 0 && _row->row() < _rowcount - 1)
    {
        log_finer("increase row counter to " << (_row->row() + 1));
        _row->row(_row->row()+1);
        return tntdb::Row(_row);
    }

    if (_rowcount > 0 && _rowcount < _fetchsize)
    {
        log_finer("last fetch returned " << _rowcount << " rows but " << _fetchsize << " was expected - finish cursor");
        _row = 0;
        return tntdb::Row();
    }

    if (_rowcount == 0 || _rowcount >= _row->row())
    {
        log_debug("OCIStmtFetch(" << _stmtp << ')');
        sword ret = OCIStmtFetch(_stmtp, _conn.getErrorHandle(), _fetchsize,
            OCI_FETCH_NEXT, OCI_DEFAULT);

        if (ret != OCI_NO_DATA && ret != OCI_SUCCESS)
            _conn.checkError(ret, "OCIStmtFetch");

        // get number of rows fetched
        ret = OCIAttrGet(_stmtp, OCI_HTYPE_STMT, &_rowcount,
            0, OCI_ATTR_ROWS_FETCHED, _conn.getErrorHandle());
        _conn.checkError(ret, "OCIAttrGet(OCI_ATTR_ROWS_FETCHED)");
        log_debug(_rowcount << " rows fetched");

        if (_rowcount == 0)
        {
            _row = 0;
            return tntdb::Row();
        }
        else
        {
            _row->row(0);
            return tntdb::Row(_row);
        }
    }
}
}
}
