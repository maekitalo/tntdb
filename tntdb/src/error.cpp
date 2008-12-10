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

#include <tntdb/error.h>
#include <cxxtools/log.h>

log_define("tntdb.error")

namespace tntdb
{
  Error::Error(const std::string& msg)
    : std::runtime_error(msg)
  {
    log_debug("Error: " << what());
  }

  NotFound::NotFound()
    : Error("not found")
  { }

  NullValue::NullValue()
    : Error("null value")
    { }

  TypeError::TypeError(const std::string& msg)
    : Error(msg)
    { }

  SqlError::SqlError(const std::string& sql_, const std::string& msg)
    : Error(msg),
      sql(sql_)
    { }

  FieldNotFound::FieldNotFound(const std::string& field_)
    : Error("field \"" + field_ + "\" not found"),
      field(field_)
  {
  }
}
