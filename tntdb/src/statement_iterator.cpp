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

#include <tntdb/bits/statement_iterator.h>
#include <cxxtools/log.h>

log_define("tntdb.statement.iterator")

namespace tntdb
{
  Statement::const_iterator::const_iterator(ICursor* cursor_)
  {
    if (cursor_)
    {
      log_debug("initial fetch");
      current = cursor_->fetch();
      if (!current)
        log_debug("no row fetched");
      else
        cursor = cursor_;
    }
  }

  Statement::const_iterator& Statement::const_iterator::operator++()
  {
    log_debug("fetch next row");
    current = cursor->fetch();
    if (!current)
    {
      log_debug("no more rows");
      cursor = 0;
    }
    return *this;
  }
}
