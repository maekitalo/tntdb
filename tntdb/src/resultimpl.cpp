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

#include <tntdb/impl/result.h>
#include <tntdb/row.h>
#include <cxxtools/log.h>

log_define("tntdb.result")

namespace tntdb
{
  Row ResultImpl::getRow(size_type tup_num) const
  {
    return data[tup_num];
  }

  ResultImpl::size_type ResultImpl::size() const
  {
    return data.size();
  }

  ResultImpl::size_type ResultImpl::getFieldCount() const
  {
    return data.empty() ? 0 : data.begin()->size();
  }
}
