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

#include <tntdb/statement.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <cxxtools/log.h>

log_define("tntdb.statement")

namespace tntdb
{
  Statement::size_type Statement::execute()
  {
    log_debug("Statement::execute()");
    return stmt->execute();
  }

  Result Statement::select()
  {
    log_debug("Statement::select()");
    return stmt->select();
  }

  Row Statement::selectRow()
  {
    log_debug("Statement::selectRow()");
    return stmt->selectRow();
  }

  Value Statement::selectValue()
  {
    log_debug("Statement::selectValue()");
    return stmt->selectValue();
  }

  Statement::const_iterator Statement::begin() const
  {
    return const_iterator(stmt->createCursor());
  }

  Statement::const_iterator Statement::end() const
  {
    return const_iterator();
  }
}
