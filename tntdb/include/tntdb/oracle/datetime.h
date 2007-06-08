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

#ifndef TNTDB_ORACLE_DATETIME_H
#define TNTDB_ORACLE_DATETIME_H

#include <tntdb/datetime.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/smartptr.h>
#include <tntdb/oracle/connection.h>
#include <oci.h>

namespace tntdb
{
  namespace oracle
  {
    class Datetime
    {
        SmartPtr<Connection, InternalRefCounted> conn;
        OCIDateTime* datetime;
        bool release;

        // low-level wrappers
        void ociDescriptorAlloc();
        void ociDescriptorFree();
        void ociConstruct(sb2 year, ub1 month, ub1 day, ub1 hour, ub1 min, ub1 sec, ub4 fsec);
        void ociAssign(OCIDateTime* src);
        void ociGetDate(sb2 &year, ub1 &month, ub1 &day) const;
        void ociGetTime(ub1 &hour, ub1 &min, ub1 &sec, ub4 &fsec) const;

      public:
        Datetime() : datetime(0), release(false) { }
        Datetime(Connection* conn, OCIDateTime* datetime, bool release = false);
        Datetime(Connection* conn, const tntdb::Datetime& s);
        Datetime(Connection* conn, const tntdb::Date& s);
        Datetime(Connection* conn, const tntdb::Time& s);

        Datetime(const Datetime& src);
        Datetime& operator= (const Datetime& src);

        OCIDateTime*& getReference(Connection* conn);

        ~Datetime()
        {
          if (release && datetime)
            ociDescriptorFree();
        }

        tntdb::Date     getDate() const;
        tntdb::Datetime getDatetime() const;
        tntdb::Time     getTime() const;

        OCIDateTime* getHandle() const  { return datetime; }
    };

  }
}

#endif // TNTDB_ORACLE_DATETIME_H

