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

#include <tntdb/oracle/datetime.h>
#include <cxxtools/log.h>

log_define("tntdb.oracle.datetime")

namespace tntdb
{
  namespace oracle
  {
    // oci-wrappers

    void Datetime::ociDescriptorAlloc()
    {
      log_debug("OCIDescriptorAlloc(OCI_DTYPE_TIMESTAMP)");
      sword ret = OCIDescriptorAlloc(conn->getEnvHandle(),
        reinterpret_cast<void**>(&datetime), OCI_DTYPE_TIMESTAMP, 0, 0);
      conn->checkError(ret, "OCIDescriptorAlloc(OCI_DTYPE_TIMESTAMP)");
    }

    void Datetime::ociDescriptorFree()
    {
      log_debug("OCIDescriptorFree(" << datetime << ", OCI_DTYPE_TIMESTAMP)");
      OCIDescriptorFree(datetime, OCI_DTYPE_TIMESTAMP);
    }

    void Datetime::ociConstruct(sb2 year, ub1 month, ub1 day, ub1 hour, ub1 min, ub1 sec, ub4 fsec)
    {
      log_debug("OCIDateTimeConstruct(" << conn->getEnvHandle() << ')');
      sword ret = OCIDateTimeConstruct(conn->getEnvHandle(), conn->getErrorHandle(),
        datetime, year, month, day, hour, min, sec, fsec * 1000000, 0, 0);
      conn->checkError(ret, "OCIDateTimeConstruct");
    }

    void Datetime::ociAssign(OCIDateTime* src)
    {
      log_debug("OCIDateTimeAssign");
      sword ret = OCIDateTimeAssign(conn->getEnvHandle(),
        conn->getErrorHandle(), src, datetime);
      conn->checkError(ret, "OCIDateTimeAssign");
    }

    void Datetime::ociGetDate(sb2 &year, ub1 &month, ub1 &day) const
    {
      log_debug("OCIDateTimeGetDate(" << datetime << ')');
      sword ret = OCIDateTimeGetDate(conn->getEnvHandle(),
        conn->getErrorHandle(), datetime, &year, &month, &day);
      conn->checkError(ret, "OCIDateTimeGetDate");
    }

    void Datetime::ociGetTime(ub1 &hour, ub1 &min, ub1 &sec, ub4 &fsec) const
    {
      log_debug("OCIDateTimeGetTime(" << datetime << ')');
      sword ret = OCIDateTimeGetTime(conn->getEnvHandle(), conn->getErrorHandle(),
        datetime, &hour, &min, &sec, &fsec);
      conn->checkError(ret, "OCIDateTimeGetTime");
    }

    // ctors, dtors, ...
    //
    Datetime::Datetime(Connection* conn_, OCIDateTime* datetime_, bool release_)
      : conn(conn_),
        datetime(datetime_),
        release(release_)
    { }

    Datetime::Datetime(Connection* conn_, const tntdb::Datetime& s)
      : conn(conn_), datetime(0), release(true)
    {
      log_debug("create oracle::Datetime from tntdb::Datetime");

      ociDescriptorAlloc();
      ociConstruct(s.getYear(), s.getMonth(), s.getDay(),
        s.getHour(), s.getMinute(), s.getSecond(), s.getMillis());
    }

    Datetime::Datetime(Connection* conn_, const tntdb::Date& s)
      : conn(conn_), datetime(0), release(true)
    {
      log_debug("create oracle::Datetime from tntdb::Date");

      ociDescriptorAlloc();
      ociConstruct(s.getYear(), s.getMonth(), s.getDay(), 0, 0, 0, 0);
    }

    Datetime::Datetime(Connection* conn_, const tntdb::Time& s)
      : conn(conn_), datetime(0), release(true)
    {
      log_debug("create oracle::Datetime from tntdb::Time");

      ociDescriptorAlloc();
      ociConstruct(0, 0, 0,
        s.getHour(), s.getMinute(), s.getSecond(), s.getMillis());
    }

    Datetime::Datetime(const Datetime& src)
      : conn(src.conn), datetime(0), release(true)
    {
      log_debug("copy oracle::Datetime");

      if (src.datetime)
      {
        ociDescriptorAlloc();
        ociAssign(src.datetime);
      }
    }

    Datetime& Datetime::operator= (const Datetime& src)
    {
      log_debug("assign oracle::Datetime");

      conn = src.conn;

      if (src.datetime)
      {
        if (datetime == 0 || !release)
        {
          // we have no handle or don't own it - create new one
          log_debug("allocate new descriptor");
          ociDescriptorAlloc();
        }

        log_debug("do assing");
        ociAssign(src.datetime);
      }
      else if (datetime)
      {
        if (release)
        {
          // free
          log_debug("release old descriptor");
          ociDescriptorFree();
        }

        datetime = 0;
      }

      log_debug("assign end");
      return *this;
    }

    // getters

    OCIDateTime*& Datetime::getReference(Connection* conn_)
    {
      if (datetime == 0)
      {
        conn = conn_;
        release = true;
        ociDescriptorAlloc();
      }
      return datetime;
    }

    tntdb::Date Datetime::getDate() const
    {
      sb2 year;
      ub1 month, day;

      ociGetDate(year, month, day);

      return tntdb::Date(year, month, day);
    }

    tntdb::Datetime Datetime::getDatetime() const
    {
      sb2 year;
      ub1 month, day, hour, min, sec;
      ub4 fsec;

      ociGetDate(year, month, day);
      ociGetTime(hour, min, sec, fsec);

      return tntdb::Datetime(year, month, day, hour, min, sec, fsec / 1000000);
    }

    tntdb::Time Datetime::getTime() const
    {
      ub1 hour, min, sec;
      ub4 fsec;

      ociGetTime(hour, min, sec, fsec);

      return tntdb::Time(hour, min, sec, fsec / 1000000);
    }

  }
}
