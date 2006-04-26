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

#include <tntdb/result.h>
#include <tntdb/value.h>
#include <tntdb/error.h>
#include <cxxtools/log.h>

log_define("tntdb.result")

namespace tntdb
{
  Row Result::getRow(size_type tup_num) const
  {
    return result->getRow(tup_num);
  }

  Value Result::getValue(size_type tup_num, size_type field_num) const
  {
    return getRow(tup_num).getValue(field_num);
  }

  Row Result::operator[] (size_type row_num) const
  {
    return getRow(row_num);
  }

  Result::const_iterator Result::begin() const
  {
    log_debug("Result::begin()");
    return const_iterator(*this, 0);
  }

  Result::const_iterator Result::end() const
  {
    size_type s = size();
    log_debug("Result::end(), size=" << s);
    return const_iterator(*this, s);
  }

}
