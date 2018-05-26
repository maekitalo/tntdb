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

#include <tntdb/odbc/error.h>

#include <cxxtools/log.h>

#include <vector>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

log_define("tntdb.odbc.error")

namespace tntdb
{
namespace odbc
{

std::string Error::getErrorMessage(
    Error& error,
    SQLSMALLINT handleType, SQLHANDLE handle)
{
    SQLRETURN ret;
    std::vector<SQLCHAR> messageText(20);
    SQLSMALLINT textLength = 0;

    log_debug("SQLGetDiagRec");

    ret = SQLGetDiagRec(handleType, handle, 1,
        error._SQLState, &error._nativeError,
        &messageText[0], messageText.size(), &textLength);

    if (textLength >= static_cast<SQLSMALLINT>(messageText.size()))
    {
        log_debug("text length " << textLength << "; SQLGetDiagRec");
        messageText.resize(textLength + 1);

        ret = SQLGetDiagRec(handleType, handle, 1,
            error._SQLState, &error._nativeError,
            &messageText[0], messageText.size(), &textLength);
    }

    std::string message(messageText.begin(), messageText.begin() + textLength);

    log_debug("diag " << &error._SQLState[0] << " native error: " << error._nativeError
        << " messageLength: " << textLength << " message: " << message);

    return message;
}

Error::Error(const std::string& message, SQLSMALLINT retval,
    SQLSMALLINT handleType, SQLHANDLE handle)
    : tntdb::Error(message + ": " + getErrorMessage(*this, handleType, handle)),
      _retval(retval)
{
}

Error::Error(SQLSMALLINT retval,
        SQLSMALLINT handleType, SQLHANDLE handle)
    : tntdb::Error(getErrorMessage(*this, handleType, handle)),
      _retval(retval)
{
}

}
}
