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
#include <tntdb/transaction.h>
#include <tntdb/statement.h>
#include <tntdb/row.h>
#include <tntdb/value.h>

log_define("tntdb.unit.base")

class TntdbBaseTest : public cxxtools::unit::TestSuite
{
    tntdb::Connection conn;
    tntdb::Statement del;

  public:
    TntdbBaseTest()
      : cxxtools::unit::TestSuite("base")
    {
      registerMethod("testSelectValue", *this, &TntdbBaseTest::testSelectValue);
      registerMethod("testSelectRow", *this, &TntdbBaseTest::testSelectRow);
      registerMethod("testRowreader", *this, &TntdbBaseTest::testRowreader);
      registerMethod("testSelectResult", *this, &TntdbBaseTest::testSelectResult);
      registerMethod("testStmtSelectValue", *this, &TntdbBaseTest::testStmtSelectValue);
      registerMethod("testStmtSelectRow", *this, &TntdbBaseTest::testStmtSelectRow);
      registerMethod("testStmtSelectResult", *this, &TntdbBaseTest::testStmtSelectResult);
      registerMethod("testStmtSelectCursor", *this, &TntdbBaseTest::testStmtSelectCursor);
      registerMethod("testExecPlaceholder", *this, &TntdbBaseTest::testExecPlaceholder);
      registerMethod("testSelectPlaceholder", *this, &TntdbBaseTest::testSelectPlaceholder);
      registerMethod("testSelectMultiplePlaceholder", *this, &TntdbBaseTest::testSelectMultiplePlaceholder);
      registerMethod("testSelectCursorPlaceholder", *this, &TntdbBaseTest::testSelectCursorPlaceholder);
      registerMethod("testTransaction", *this, &TntdbBaseTest::testTransaction);
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
        del.execute();
      }
    }

    void tearDown()
    {
      del.execute();
    }

    void testSelectValue()
    {
      conn.execute("insert into tntdbtest(intcol) values(4)");
      conn.execute("insert into tntdbtest(intcol) values(5)");
      conn.execute("insert into tntdbtest(intcol) values(3)");

      int maxVal = 0;
      bool notNull = conn.selectValue("select max(intcol) from tntdbtest").get(maxVal);
      CXXTOOLS_UNIT_ASSERT(notNull);
      CXXTOOLS_UNIT_ASSERT_EQUALS(maxVal, 5);
    }

    void testSelectRow()
    {
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(4, 5, 6)");

      tntdb::Row r = conn.selectRow("select intcol, shortcol, longcol, unsignedshortcol from tntdbtest");

      CXXTOOLS_UNIT_ASSERT_EQUALS(r.size(), 4);

      int intVal = 0;
      short shortVal = 0;
      long longVal = 0;
      unsigned short ushortVal = 0;

      bool intNotNull = r[0].get(intVal);
      bool shortNotNull = r[1].get(shortVal);
      bool longNotNull = r[2].get(longVal);
      bool ushortNotNull = r[3].get(ushortVal);

      CXXTOOLS_UNIT_ASSERT(intNotNull);
      CXXTOOLS_UNIT_ASSERT(shortNotNull);
      CXXTOOLS_UNIT_ASSERT(longNotNull);
      CXXTOOLS_UNIT_ASSERT(!ushortNotNull);
      CXXTOOLS_UNIT_ASSERT_EQUALS(intVal, 4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(shortVal, 5);
      CXXTOOLS_UNIT_ASSERT_EQUALS(longVal, 6);
    }

    void testRowreader()
    {
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(4, 5, 6)");

      tntdb::Row r = conn.selectRow("select intcol, shortcol, longcol, unsignedshortcol from tntdbtest");

      CXXTOOLS_UNIT_ASSERT_EQUALS(r.size(), 4);

      int intVal = 0;
      short shortVal = 0;
      long longVal = 0;
      bool longNotNull = false;
      unsigned short ushortVal = 0;
      bool ushortNotNull = true;

      r.reader().get(intVal)
                .get(shortVal)
                .get(longVal, longNotNull)
                .get(ushortVal, ushortNotNull);

      CXXTOOLS_UNIT_ASSERT(longNotNull);
      CXXTOOLS_UNIT_ASSERT(!ushortNotNull);
      CXXTOOLS_UNIT_ASSERT_EQUALS(intVal, 4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(shortVal, 5);
      CXXTOOLS_UNIT_ASSERT_EQUALS(longVal, 6);
    }

    void testSelectResult()
    {
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(4, 5, 6)");
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(3, 4, 5)");

      tntdb::Result r = conn.select("select intcol, shortcol, longcol, unsignedshortcol from tntdbtest order by intcol");

      CXXTOOLS_UNIT_ASSERT_EQUALS(r.size(), 2);
      CXXTOOLS_UNIT_ASSERT_EQUALS(r[0].size(), 4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(r[1].size(), 4);

      int intVal = 0;
      short shortVal = 0;
      long longVal = 0;
      unsigned short ushortVal = 0;

      bool intNotNull = r[0][0].get(intVal);
      bool shortNotNull = r[0][1].get(shortVal);
      bool longNotNull = r[0][2].get(longVal);
      bool ushortNotNull = r[0][3].get(ushortVal);

      CXXTOOLS_UNIT_ASSERT(intNotNull);
      CXXTOOLS_UNIT_ASSERT(shortNotNull);
      CXXTOOLS_UNIT_ASSERT(longNotNull);
      CXXTOOLS_UNIT_ASSERT(!ushortNotNull);
      CXXTOOLS_UNIT_ASSERT_EQUALS(intVal, 3);
      CXXTOOLS_UNIT_ASSERT_EQUALS(shortVal, 4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(longVal, 5);

      intNotNull = r[1][0].get(intVal);
      shortNotNull = r[1][1].get(shortVal);
      longNotNull = r[1][2].get(longVal);
      ushortNotNull = r[1][3].get(ushortVal);

      CXXTOOLS_UNIT_ASSERT(intNotNull);
      CXXTOOLS_UNIT_ASSERT(shortNotNull);
      CXXTOOLS_UNIT_ASSERT(longNotNull);
      CXXTOOLS_UNIT_ASSERT(!ushortNotNull);
      CXXTOOLS_UNIT_ASSERT_EQUALS(intVal, 4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(shortVal, 5);
      CXXTOOLS_UNIT_ASSERT_EQUALS(longVal, 6);

    }

    void testStmtSelectValue()
    {
      conn.execute("insert into tntdbtest(intcol) values(4)");
      conn.execute("insert into tntdbtest(intcol) values(5)");
      conn.execute("insert into tntdbtest(intcol) values(3)");

      int maxVal = 0;
      bool notNull = conn.prepare("select max(intcol) from tntdbtest").selectValue().get(maxVal);
      CXXTOOLS_UNIT_ASSERT(notNull);
      CXXTOOLS_UNIT_ASSERT_EQUALS(maxVal, 5);
    }

    void testStmtSelectRow()
    {
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(4, 5, 6)");

      tntdb::Row r = conn.prepare("select intcol, shortcol, longcol, unsignedshortcol from tntdbtest").selectRow();

      CXXTOOLS_UNIT_ASSERT_EQUALS(r.size(), 4);

      int intVal = 0;
      short shortVal = 0;
      long longVal = 0;
      unsigned short ushortVal = 0;

      bool intNotNull = r[0].get(intVal);
      bool shortNotNull = r[1].get(shortVal);
      bool longNotNull = r[2].get(longVal);
      bool ushortNotNull = r[3].get(ushortVal);

      CXXTOOLS_UNIT_ASSERT(intNotNull);
      CXXTOOLS_UNIT_ASSERT(shortNotNull);
      CXXTOOLS_UNIT_ASSERT(longNotNull);
      CXXTOOLS_UNIT_ASSERT(!ushortNotNull);
      CXXTOOLS_UNIT_ASSERT_EQUALS(intVal, 4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(shortVal, 5);
      CXXTOOLS_UNIT_ASSERT_EQUALS(longVal, 6);
    }

    void testStmtSelectResult()
    {
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(4, 5, 6)");
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(3, 4, 5)");

      tntdb::Result r = conn.prepare("select intcol, shortcol, longcol, unsignedshortcol from tntdbtest order by intcol").select();

      CXXTOOLS_UNIT_ASSERT_EQUALS(r.size(), 2);
      CXXTOOLS_UNIT_ASSERT_EQUALS(r[0].size(), 4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(r[1].size(), 4);

      int intVal = 0;
      short shortVal = 0;
      long longVal = 0;
      unsigned short ushortVal = 0;

      bool intNotNull = r[0][0].get(intVal);
      bool shortNotNull = r[0][1].get(shortVal);
      bool longNotNull = r[0][2].get(longVal);
      bool ushortNotNull = r[0][3].get(ushortVal);

      CXXTOOLS_UNIT_ASSERT(intNotNull);
      CXXTOOLS_UNIT_ASSERT(shortNotNull);
      CXXTOOLS_UNIT_ASSERT(longNotNull);
      CXXTOOLS_UNIT_ASSERT(!ushortNotNull);
      CXXTOOLS_UNIT_ASSERT_EQUALS(intVal, 3);
      CXXTOOLS_UNIT_ASSERT_EQUALS(shortVal, 4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(longVal, 5);

      intNotNull = r[1][0].get(intVal);
      shortNotNull = r[1][1].get(shortVal);
      longNotNull = r[1][2].get(longVal);
      ushortNotNull = r[1][3].get(ushortVal);

      CXXTOOLS_UNIT_ASSERT(intNotNull);
      CXXTOOLS_UNIT_ASSERT(shortNotNull);
      CXXTOOLS_UNIT_ASSERT(longNotNull);
      CXXTOOLS_UNIT_ASSERT(!ushortNotNull);
      CXXTOOLS_UNIT_ASSERT_EQUALS(intVal, 4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(shortVal, 5);
      CXXTOOLS_UNIT_ASSERT_EQUALS(longVal, 6);

    }

    void testStmtSelectCursor()
    {
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(4, 5, 6)");
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(3, 4, 5)");

      tntdb::Statement stmt = conn.prepare("select intcol, shortcol, longcol, unsignedshortcol from tntdbtest order by intcol");

      unsigned rowcount = 0;
      for (tntdb::Statement::const_iterator cur = stmt.begin(); cur != stmt.end(); ++cur, ++rowcount)
      {
        tntdb::Row row = *cur;
        int intVal = 0;
        short shortVal = 0;
        long longVal = 0;
        unsigned short ushortVal = 0;

        bool intNotNull = row[0].get(intVal);
        bool shortNotNull = row[1].get(shortVal);
        bool longNotNull = row[2].get(longVal);
        bool ushortNotNull = row[3].get(ushortVal);

        CXXTOOLS_UNIT_ASSERT(intNotNull);
        CXXTOOLS_UNIT_ASSERT(shortNotNull);
        CXXTOOLS_UNIT_ASSERT(longNotNull);
        CXXTOOLS_UNIT_ASSERT(!ushortNotNull);
        CXXTOOLS_UNIT_ASSERT_EQUALS(intVal, rowcount + 3);
        CXXTOOLS_UNIT_ASSERT_EQUALS(shortVal, rowcount + 4);
        CXXTOOLS_UNIT_ASSERT_EQUALS(longVal, rowcount + 5);
      }
    }

    void testExecPlaceholder()
    {
      tntdb::Statement ins = conn.prepare("insert into tntdbtest(intcol) values(:intcol)");
      ins.set("intcol", 4).execute();
      ins.set("intcol", 5).execute();
      ins.set("intcol", 3).execute();

      int maxVal = 0;
      bool notNull = conn.selectValue("select max(intcol) from tntdbtest").get(maxVal);
      CXXTOOLS_UNIT_ASSERT(notNull);
      CXXTOOLS_UNIT_ASSERT_EQUALS(maxVal, 5);
    }

    void testSelectPlaceholder()
    {
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(4, 5, 6)");
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(3, 4, 5)");
      tntdb::Statement sel = conn.prepare("select shortcol from tntdbtest where intcol = :intcol");
      short shortVal = 0;
      bool shortNotNull = sel.set("intcol", 3).selectValue().get(shortVal);
      CXXTOOLS_UNIT_ASSERT(shortNotNull);
      CXXTOOLS_UNIT_ASSERT_EQUALS(shortVal, 4);
    }

    void testSelectMultiplePlaceholder()
    {
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(4, 5, 6)");
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(3, 4, 5)");
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(5, 6, 7)");
      tntdb::Statement sel = conn.prepare("select count(*) from tntdbtest where intcol = :value or shortcol = :value");
      unsigned count = 0;
      sel.set("value", 4).selectValue().get(count);
      CXXTOOLS_UNIT_ASSERT_EQUALS(count, 2);
    }

    void testSelectCursorPlaceholder()
    {
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(5, 6, 7)");
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(4, 5, 6)");
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(3, 4, 5)");

      tntdb::Statement stmt = conn.prepare("select intcol, shortcol, longcol, unsignedshortcol from tntdbtest where intcol >= :intcol order by intcol");
      stmt.set("intcol", 4);

      unsigned rowcount = 0;
      for (tntdb::Statement::const_iterator cur = stmt.begin(); cur != stmt.end(); ++cur, ++rowcount)
      {
        tntdb::Row row = *cur;
        int intVal = 0;
        short shortVal = 0;
        long longVal = 0;
        unsigned short ushortVal = 0;

        bool intNotNull = row[0].get(intVal);
        bool shortNotNull = row[1].get(shortVal);
        bool longNotNull = row[2].get(longVal);
        bool ushortNotNull = row[3].get(ushortVal);

        CXXTOOLS_UNIT_ASSERT(intNotNull);
        CXXTOOLS_UNIT_ASSERT(shortNotNull);
        CXXTOOLS_UNIT_ASSERT(longNotNull);
        CXXTOOLS_UNIT_ASSERT(!ushortNotNull);
        CXXTOOLS_UNIT_ASSERT_EQUALS(intVal, rowcount + 4);
        CXXTOOLS_UNIT_ASSERT_EQUALS(shortVal, rowcount + 5);
        CXXTOOLS_UNIT_ASSERT_EQUALS(longVal, rowcount + 6);
      }
    }

    void testTransaction()
    {
      unsigned count = 0;

      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(4, 5, 6)");
      conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(3, 4, 5)");

      {
        tntdb::Transaction trans(conn);
        conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(5, 6, 7)");

        conn.selectValue("select count(*) from tntdbtest").get(count);
        CXXTOOLS_UNIT_ASSERT_EQUALS(count, 3);
      }

      conn.selectValue("select count(*) from tntdbtest").get(count);
      CXXTOOLS_UNIT_ASSERT_EQUALS(count, 2);

      {
        tntdb::Transaction trans(conn);
        conn.execute("insert into tntdbtest(intcol, shortcol, longcol) values(5, 6, 7)");

        conn.selectValue("select count(*) from tntdbtest").get(count);
        CXXTOOLS_UNIT_ASSERT_EQUALS(count, 3);

        trans.commit();
      }

      conn.selectValue("select count(*) from tntdbtest").get(count);
      CXXTOOLS_UNIT_ASSERT_EQUALS(count, 3);

    }

};

cxxtools::unit::RegisterTest<TntdbBaseTest> register_TntdbBaseTest;
