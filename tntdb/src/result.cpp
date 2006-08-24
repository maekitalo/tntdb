/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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
