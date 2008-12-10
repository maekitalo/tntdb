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
#include <tntdb/oracle/statement.h>
#include <cxxtools/log.h>

log_define("tntdb.oracle.cursor")

namespace tntdb
{
  namespace oracle
  {
    Cursor::Cursor(Statement* stmt_, unsigned)
      : stmt(stmt_)
    {
      log_debug("execute statement " << stmt->getHandle());
      sword ret = OCIStmtExecute(stmt->getConnection()->getSvcCtxHandle(),
        stmt->getHandle(), stmt->getErrorHandle(), 0, 0, 0, 0, OCI_DEFAULT);
      stmt->checkError(ret, "OCIStmtExecute");

      row = tntdb::Row(new oracle::Row(stmt));

      stmtp = stmt->getHandle();
      stmt->stmtp = 0;
    }

    Cursor::~Cursor()
    {
      if (stmtp)
      {
        if (stmt->stmtp)
        {
          log_debug("release statement handle " << stmtp);
          sword ret = OCIHandleFree(stmtp, OCI_HTYPE_STMT);
          if (ret != OCI_SUCCESS)
            log_error("OCIHandleFree(" << stmtp << " OCI_HTYPE_STMT) failed");
        }
        else
        {
          stmt->stmtp = stmtp;
        }
      }
    }

    tntdb::Row Cursor::fetch()
    {
      log_debug("OCIStmtFetch(" << stmtp << ')');
      sword ret = OCIStmtFetch(stmtp, stmt->getErrorHandle(), 1,
          OCI_FETCH_NEXT, OCI_DEFAULT);
      if (ret == OCI_NO_DATA)
        row = tntdb::Row();
      else
        stmt->checkError(ret, "OCIStmtFetch");

      return row;
    }
  }
}
