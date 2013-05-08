/*
 * Copyright (C) 2012 Tommi Maekitalo
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


#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>
#include <cxxtools/log.h>
#include <stdlib.h>
#include <tntdb/connect.h>
#include <tntdb/statement.h>
#include <tntdb/row.h>
#include <limits>

log_define("tntdb.unit.colname")

class TntdbColnameTest : public cxxtools::unit::TestSuite
{
    tntdb::Connection conn;
    tntdb::Statement del;

  public:
    TntdbColnameTest()
      : cxxtools::unit::TestSuite("colname")
    {
      registerMethod("testColname", *this, &TntdbColnameTest::testColname);
    }

    void setUp()
    {
      if (!conn)
      {
        const char* dburl = getenv("TNTDBURL");
        if (!dburl)
          dburl = "sqlite:test.db";

        log_info("testing with dburl=" << dburl);

        conn = tntdb::connect(dburl);
        del = conn.prepare("delete from tntdbtest");
      }

      del.execute();
    }

    void testColname()
    {
      conn.execute("insert into tntdbtest(intcol) values(4718)");

      tntdb::Row row = conn.selectRow("select intcol, intcol as foo from tntdbtest");

      CXXTOOLS_UNIT_ASSERT_EQUALS(row.getName(0), "intcol");
      CXXTOOLS_UNIT_ASSERT_EQUALS(row.getName(1), "foo");
    }

};

cxxtools::unit::RegisterTest<TntdbColnameTest> register_TntdbColnameTest;
