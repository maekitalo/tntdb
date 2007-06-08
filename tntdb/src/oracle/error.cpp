/* 
 * Copyright (C) 2007 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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
    namespace
    {
      std::string errorMessage(OCIError* errhp, const char* function = 0)
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

      std::string errorMessage(const std::string& str, const char* function = 0)
      {
        std::string msg = str;
        if (function)
        {
          msg += " in function ";
          msg += function;
        }
        return msg;
      }

    }

    Error::Error(OCIError* errhp, const char* function)
      : tntdb::Error(errorMessage(errhp, function))
    { }

    Error::Error(const std::string& msg, const char* function)
      : tntdb::Error(errorMessage(msg, function))
    { }
  }
}
