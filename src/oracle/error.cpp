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

#include <tntdb/oracle/error.h>
#include <sstream>
#include <oci.h>
#include <cxxtools/log.h>

log_define("tntdb.oracle.error")

namespace tntdb
{
namespace oracle
{
std::string Error::errorMessage(OCIError* errhp, const char* function)
{
    int errcode;
    char errbuf[512];
    OCIErrorGet((dvoid*)errhp, (ub4)1, (text*)NULL, &errcode,
                (unsigned char*)errbuf, (ub4)sizeof(errbuf), OCI_HTYPE_ERROR);

    std::ostringstream msg;
    msg << "oracle error " << errcode;
    if (function)
      msg << " in " << function;
    msg << ": " << errbuf;
    return msg.str();
}

std::string Error::errorMessage(const std::string& str, const char* function)
{
    std::string msg = str;
    if (function)
    {
        msg += " in function ";
        msg += function;
    }
    return msg;
}

Error::Error(OCIError* errhp, const char* function)
  : tntdb::Error(errorMessage(errhp, function))
{ }

Error::Error(const std::string& msg, const char* function)
  : tntdb::Error(errorMessage(msg, function))
{ }

namespace error
{
void checkError(OCIError* errhp, sword ret, const char* function)
{
    switch (ret)
    {
        case OCI_SUCCESS:
            break;

        case OCI_SUCCESS_WITH_INFO:
            log_warn(function << ": OCI_SUCCESS_WITH_INFO: " << Error::errorMessage(errhp, function));
            break;

        case OCI_NEED_DATA:
            log_warn(function << ": OCI_NEED_DATA");
            throw Error(errhp, function);

        case OCI_NO_DATA:
            log_debug(function << ": OCI_NO_DATA");
            throw NotFound();

        case OCI_ERROR:
            throw Error(errhp, function);

        case OCI_INVALID_HANDLE:
            log_error("OCI_INVALID_HANDLE");
            throw InvalidHandle(function);

        case OCI_STILL_EXECUTING:
            log_error("OCI_STILL_EXECUTING");
            throw StillExecuting(function);

        case OCI_CONTINUE:
            log_error("OCI_CONTINUE");
            throw ErrorContinue(function);
    }
}
}

}
}
