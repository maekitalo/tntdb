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

#ifndef TNTDB_ORACLE_ERROR_H
#define TNTDB_ORACLE_ERROR_H

#include <tntdb/error.h>
#include <oci.h>

namespace tntdb
{
  namespace oracle
  {
    class Error : public tntdb::Error
    {
      public:
        explicit Error(OCIError* errhp, const char* function = 0);
        explicit Error(const std::string& msg, const char* function = 0);
    };

    class InvalidHandle : public Error
    {
      public:
        explicit InvalidHandle(const char* function = 0)
          : Error("OCI_INVALID_HANDLE", function)
          { }
    };

    class StillExecuting : public Error
    {
      public:
        explicit StillExecuting(const char* function = 0)
          : Error("OCI_STILL_EXECUTING", function)
          { }
    };

    class ErrorContinue : public Error
    {
      public:
        explicit ErrorContinue(const char* function = 0)
          : Error("OCI_CONTINUE", function)
          { }
    };

  }
}

#endif // TNTDB_ORACLE_ERROR_H

