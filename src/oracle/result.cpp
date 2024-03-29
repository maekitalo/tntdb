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

#include <tntdb/oracle/result.h>
#include <tntdb/oracle/row.h>
#include <tntdb/oracle/multirow.h>
#include <tntdb/oracle/singlerow.h>
#include <cxxtools/log.h>

log_define("tntdb.oracle.result")

namespace tntdb
{
namespace oracle
{
Result::Result(Statement& stmt)
{
    sword ret;

    log_debug("execute select-statement " << stmt.getHandle());

    ret = OCIStmtExecute(stmt.getConnection().getSvcCtxHandle(),
        stmt.getHandle(), stmt.getConnection().getErrorHandle(), 0, 0, 0, 0, OCI_DEFAULT);
    stmt.getConnection().checkError(ret, "OCIStmtExecute");

    log_debug("select-statement " << stmt.getHandle() << " executed");

    // get columncount
    ret = OCIAttrGet(stmt.getHandle(), OCI_HTYPE_STMT, &_columncount,
        0, OCI_ATTR_PARAM_COUNT, stmt.getConnection().getErrorHandle());
    stmt.getConnection().checkError(ret, "OCIAttrGet(OCI_ATTR_PARAM_COUNT)");

    log_debug("define row");
    auto row = std::make_shared<Row>(stmt, _columncount);

    log_debug("fetch results");
    while ((ret = OCIStmtFetch(stmt.getHandle(), stmt.getConnection().getErrorHandle(), 1,
        OCI_FETCH_NEXT, OCI_DEFAULT)) == OCI_SUCCESS)
    {
        _rows.emplace_back(row);
        row = std::make_shared<Row>(stmt, _columncount);
    }

    if (ret != OCI_NO_DATA)
        stmt.getConnection().checkError(ret, "OCIStmtFetch");
}

Result::Result(Statement& stmt, unsigned fetchsize)
{
    sword ret;

    log_debug("execute select-statement " << stmt.getHandle());

    ret = OCIStmtExecute(stmt.getConnection().getSvcCtxHandle(),
        stmt.getHandle(), stmt.getConnection().getErrorHandle(), 0, 0, 0, 0, OCI_DEFAULT);
    stmt.getConnection().checkError(ret, "OCIStmtExecute");

    log_debug("select-statement " << stmt.getHandle() << " executed");

    // get columncount
    ret = OCIAttrGet(stmt.getHandle(), OCI_HTYPE_STMT, &_columncount,
        0, OCI_ATTR_PARAM_COUNT, stmt.getConnection().getErrorHandle());
    stmt.getConnection().checkError(ret, "OCIAttrGet(OCI_ATTR_PARAM_COUNT)");

    log_debug("define multirow");
    auto mr = std::make_shared<MultiRow>(stmt, fetchsize, _columncount);

    log_debug("fetch results");
    while ((ret = OCIStmtFetch(stmt.getHandle(), stmt.getConnection().getErrorHandle(), fetchsize,
        OCI_FETCH_NEXT, OCI_DEFAULT)) != OCI_ERROR)
    {
        // get number of rows fetched
        ub4 rowsFetched;
        sword ret = OCIAttrGet(stmt.getHandle(), OCI_HTYPE_STMT, &rowsFetched,
            0, OCI_ATTR_ROWS_FETCHED, stmt.getConnection().getErrorHandle());
        stmt.getConnection().checkError(ret, "OCIAttrGet(OCI_ATTR_ROWS_FETCHED)");

        for (unsigned n = 0; n < rowsFetched; ++n)
            _rows.emplace_back(std::make_shared<SingleRow>(mr, n));

        if (rowsFetched < fetchsize)
            return;

        mr = std::make_shared<MultiRow>(stmt, fetchsize, _columncount);
    }

    if (ret != OCI_NO_DATA && ret != OCI_SUCCESS)
        stmt.getConnection().checkError(ret, "OCIStmtFetch");
}

tntdb::Row Result::getRow(size_type tup_num) const
{
    return tntdb::Row(_rows.at(tup_num));
}

Result::size_type Result::size() const
{
    return _rows.size();
}

Result::size_type Result::getFieldCount() const
{
    return _columncount;
}

}
}
