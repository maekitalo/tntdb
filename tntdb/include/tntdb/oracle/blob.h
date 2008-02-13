/*
 * Copyright (C) 2008 Tommi Maekitalo
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
 */

#ifndef TNTDB_ORACLE_BLOB_H
#define TNTDB_ORACLE_BLOB_H

#include <tntdb/datetime.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/blob.h>
#include <cxxtools/smartptr.h>
#include <tntdb/oracle/connection.h>
#include <oci.h>

namespace tntdb
{
  namespace oracle
  {
    class Blob : private cxxtools::NonCopyable
    {
        cxxtools::SmartPtr<Connection> conn;
        OCILobLocator* lob;
        bool release;

        // low-level wrappers
        void ociDescriptorAlloc();
        void ociDescriptorFree();

      public:
        Blob() : lob(0), release(false) { }
        Blob(Connection* conn, OCILobLocator* lob, bool release = false);
        Blob(Connection* conn, const char* data, ub4 count);

        ~Blob()
        {
          if (lob)
            ociDescriptorFree();
        }

        void allocateHandle();
        void setData(Connection* conn, const char* data, ub4 count);
        void getData(tntdb::Blob& ret) const;

        OCILobLocator*& getHandle(Connection* conn);
    };

  }
}

#endif // TNTDB_ORACLE_BLOB_H

