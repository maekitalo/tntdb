/*
 * Copyright (C) 2018 Tommi Maekitalo
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

#include <tntdb/odbc/handle.h>

#include <tntdb/error.h>

#include <cxxtools/log.h>

#include <sql.h>

log_define("tntdb.odbc.handle")

namespace tntdb
{
namespace odbc
{
Handle::Handle(SQLSMALLINT handleType, SQLHANDLE inputHandle)
    : _type(handleType),
      _handle(0)
{
    long ret;

    log_debug("allocate handle of type " << _type);
    ret = SQLAllocHandle(_type, inputHandle, &_handle);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
        throw tntdb::Error("Unable to allocate a handle");
}

Handle::~Handle()
{
    if (_handle)
    {
        long ret;
        ret = SQLFreeHandle(_type, _handle);
        log_error_if(ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO, "failed to free handle");
    }
}

}
}
