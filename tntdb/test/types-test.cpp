/*
 * Copyright (C) 2010 Tommi Maekitalo
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
#include <stdlib.h>
#include <tntdb/connect.h>
#include <tntdb/statement.h>
#include <tntdb/row.h>
#include <limits>

#define BEGIN_TEST(T, col)                           \
      const std::string colName = col;               \
      tntdb::Statement ins = conn.prepare(           \
        "insert into tntdbtest(" col  ") values(:" col ")"); \
      tntdb::Statement sel = conn.prepare(           \
        "select " col " from tntdbtest");            \
      tntdb::Value dbvalue;                          \
      T res;

#define TEST(val)                                    \
      del.execute();                                 \
      ins.set(colName, val).execute();               \
      dbvalue = sel.selectValue();                   \
      dbvalue.get(res);                              \
      CXXTOOLS_UNIT_ASSERT_EQUALS(val, res);

#define TESTEQ(val)                                  \
      del.execute();                                 \
      ins.set(colName, val).execute();               \
      dbvalue = sel.selectValue();                   \
      dbvalue.get(res);                              \
      CXXTOOLS_UNIT_ASSERT(val == res);

#define TESTFLOAT(val)                               \
      del.execute();                                 \
      ins.set(colName, val).execute();               \
      dbvalue = sel.selectValue();                   \
      dbvalue.get(res);                              \
      CXXTOOLS_UNIT_ASSERT(val / res >= .9999 && val / res <= 1.0001);

#define TESTDT(val)                                  \
      del.execute();                                 \
      ins.set(colName, val).execute();               \
      dbvalue = sel.selectValue();                   \
      dbvalue.get(res);                              \
      CXXTOOLS_UNIT_ASSERT_EQUALS(val.getIso(), res.getIso());

class TntdbTypesTest : public cxxtools::unit::TestSuite
{
    const char* dburl;
    tntdb::Connection conn;
    tntdb::Statement del;

  public:
    TntdbTypesTest()
      : cxxtools::unit::TestSuite("tntdb-types-Test")
    {
      registerMethod("testLimits", *this, &TntdbTypesTest::testLimits);
      registerMethod("testNull", *this, &TntdbTypesTest::testNull);

      registerMethod("testBool", *this, &TntdbTypesTest::testBool);
      registerMethod("testShort", *this, &TntdbTypesTest::testShort);
      registerMethod("testInt", *this, &TntdbTypesTest::testInt);
      registerMethod("testLong", *this, &TntdbTypesTest::testLong);
      registerMethod("testUnsignedShort", *this, &TntdbTypesTest::testUnsignedShort);
      registerMethod("testUnsigned", *this, &TntdbTypesTest::testUnsigned);
      registerMethod("testUnsignedLong", *this, &TntdbTypesTest::testUnsignedLong);
      registerMethod("testInt32", *this, &TntdbTypesTest::testInt32);
      registerMethod("testUint32", *this, &TntdbTypesTest::testUint32);
      registerMethod("testInt64", *this, &TntdbTypesTest::testInt64);
      registerMethod("testUint64", *this, &TntdbTypesTest::testUint64);
      registerMethod("testDecimal", *this, &TntdbTypesTest::testDecimal);
      registerMethod("testFloat", *this, &TntdbTypesTest::testFloat);
      registerMethod("testDouble", *this, &TntdbTypesTest::testDouble);
      registerMethod("testChar", *this, &TntdbTypesTest::testChar);
      registerMethod("testString", *this, &TntdbTypesTest::testString);
      registerMethod("testBlob", *this, &TntdbTypesTest::testBlob);
      registerMethod("testDate", *this, &TntdbTypesTest::testDate);
      registerMethod("testTime", *this, &TntdbTypesTest::testTime);
      registerMethod("testDatetime", *this, &TntdbTypesTest::testDatetime);
      registerMethod("testSequence", *this, &TntdbTypesTest::testSequence);
      registerMethod("testFloatNan", *this, &TntdbTypesTest::testFloatNan);
      registerMethod("testDoubleNan", *this, &TntdbTypesTest::testDoubleNan);

      dburl = getenv("TNTDBURL");
      if (!dburl)
        dburl = "sqlite:test.db";

      std::cout << "testing with dburl=" << dburl << std::endl;
    }

    void setUp()
    {
      if (!conn)
      {
        conn = tntdb::connect(dburl);
        del = conn.prepare("delete from tntdbtest");
      }
    }

    void testBool()
    {
      BEGIN_TEST(bool, "boolcol");
      TEST(true);
      TEST(false);
    }

    void testShort()
    {
      BEGIN_TEST(short, "shortcol");
      TEST(static_cast<short>(42));
      TEST(static_cast<short>(-123));
      TEST(std::numeric_limits<short>::max());
      TEST(std::numeric_limits<short>::min());
    }

    void testInt()
    {
      BEGIN_TEST(int, "intcol");
      TEST(static_cast<int>(42));
      TEST(static_cast<int>(-123));
      TEST(std::numeric_limits<int>::max());
      TEST(std::numeric_limits<int>::min());
    }

    void testLong()
    {
      BEGIN_TEST(long, "longcol");
      TEST(static_cast<long>(42));
      TEST(static_cast<long>(-123));
      TEST(std::numeric_limits<long>::max());
      TEST(std::numeric_limits<long>::min());
    }

    void testUnsignedShort()
    {
      BEGIN_TEST(unsigned short, "unsignedshortcol");
      TEST(static_cast<unsigned short>(42));
      TEST(std::numeric_limits<unsigned short>::max());
      TEST(std::numeric_limits<unsigned short>::min());
    }

    void testUnsigned()
    {
      BEGIN_TEST(unsigned, "unsignedcol");
      TEST(static_cast<unsigned>(42));
      TEST(std::numeric_limits<unsigned>::max());
      TEST(std::numeric_limits<unsigned>::min());
    }

    void testUnsignedLong()
    {
      BEGIN_TEST(unsigned long, "unsignedlongcol");
      TEST(static_cast<unsigned long>(42));
      TEST(std::numeric_limits<unsigned long>::max());
      TEST(std::numeric_limits<unsigned long>::min());
    }

    void testInt32()
    {
      BEGIN_TEST(int32_t, "int32col");
      TEST(static_cast<int32_t>(42));
      TEST(static_cast<int32_t>(-123));
      TEST(std::numeric_limits<int32_t>::max());
      TEST(std::numeric_limits<int32_t>::min());
    }

    void testUint32()
    {
      BEGIN_TEST(uint32_t, "uint32col");
      TEST(static_cast<uint32_t>(42));
      TEST(std::numeric_limits<uint32_t>::max());
      TEST(std::numeric_limits<uint32_t>::min());
    }

    void testInt64()
    {
      BEGIN_TEST(int64_t, "int64col");
      TEST(static_cast<int64_t>(42));
      TEST(static_cast<int64_t>(-123));
      TEST(std::numeric_limits<int64_t>::max());
      TEST(std::numeric_limits<int64_t>::min());
    }

    void testUint64()
    {
      BEGIN_TEST(uint64_t, "uint64col");
      TEST(static_cast<uint64_t>(42));
      TEST(std::numeric_limits<uint64_t>::max());
      TEST(std::numeric_limits<uint64_t>::min());
    }

    void testDecimal()
    {
      BEGIN_TEST(tntdb::Decimal, "decimalcol");
      TEST(tntdb::Decimal(456, 8));
      TEST(tntdb::Decimal(456, -4));
      TEST(tntdb::Decimal(-456, 8));
    }

    void testFloat()
    {
      BEGIN_TEST(float, "floatcol");
      TESTFLOAT(static_cast<float>(42));
      TESTFLOAT(static_cast<float>(-123));
      TESTFLOAT(std::numeric_limits<float>::max() * .999999);
      TESTFLOAT(std::numeric_limits<float>::min() * 1.00001);
      TEST(std::numeric_limits<double>::infinity());
      TEST(-std::numeric_limits<double>::infinity());
    }

    void testDouble()
    {
      BEGIN_TEST(double, "doublecol");
      TESTFLOAT(static_cast<double>(42));
      TESTFLOAT(static_cast<double>(-123));
      TESTFLOAT(std::numeric_limits<double>::max() * .999999);
      TESTFLOAT(std::numeric_limits<double>::min() * 1.00001);
      TEST(std::numeric_limits<double>::infinity());
      TEST(-std::numeric_limits<double>::infinity());
    }

    void testChar()
    {
      BEGIN_TEST(char, "charcol");
      TEST('A');
      TEST('Z');
      TEST('\n');
    }

    void testString()
    {
      BEGIN_TEST(std::string, "stringcol");
      TEST(std::string("Hello\n"));
      TEST(std::string("M\xc3\xa4kitalo"));
    }

    void testBlob()
    {
      BEGIN_TEST(tntdb::Blob, "blobcol");
      tntdb::Blob blobval = tntdb::Blob("\0\1\2\3\0xff", 5);
      TESTEQ(blobval);
    }

    void testDate()
    {
      BEGIN_TEST(tntdb::Date, "datecol");
      TESTDT(tntdb::Date());
      TESTDT(tntdb::Date(2010, 2, 15));
    }

    void testTime()
    {
      BEGIN_TEST(tntdb::Time, "timecol");
      TESTDT(tntdb::Time());
      TESTDT(tntdb::Time(20, 9, 31, 12));
    }

    void testDatetime()
    {
      BEGIN_TEST(tntdb::Datetime, "datetimecol");
      TESTDT(tntdb::Datetime());
      TESTDT(tntdb::Datetime(2010, 2, 15, 20, 9, 31, 12));
    }

    void testSequence()
    {
      del.execute();

      tntdb::Statement ins = conn.prepare(
        "insert into tntdbtest(intcol) values(:intcol)");

      ins.set("intcol", 42)
         .execute();

      long serialval = conn.lastInsertId("tntdbtest_seq");

      tntdb::Statement sel = conn.prepare(
        "select id from tntdbtest");

      tntdb::Value dbvalue = sel.selectValue();

      long serialres = -1;
      dbvalue.get(serialres);
      CXXTOOLS_UNIT_ASSERT_EQUALS(serialval, serialres);
    }

    void testLimits()
    {
      del.execute();

      tntdb::Statement ins = conn.prepare(
        "insert into tntdbtest("
        "    intcol, longcol, unsignedcol, unsignedlongcol,"
        "    int32col, uint32col, int64col, uint64col,"
        "    floatcol, doublecol)"
        " values("
        "    :intcol, :longcol, :unsignedcol, :unsignedlongcol,"
        "    :int32col, :uint32col, :int64col, :uint64col,"
        "    :floatcol, :doublecol)");

      int intval                  = std::numeric_limits<int>::max();
      long longval                = std::numeric_limits<long>::max();
      unsigned uval               = std::numeric_limits<unsigned>::max();
      unsigned long ulongval      = std::numeric_limits<unsigned long>::max();
      int32_t int32val            = std::numeric_limits<int32_t>::max();
      uint32_t uint32val          = std::numeric_limits<uint32_t>::max();
      int64_t int64val            = std::numeric_limits<int64_t>::max();
      uint64_t uint64val          = std::numeric_limits<uint64_t>::max();
      float floatval              = std::numeric_limits<float>::max() * .999999;
      double doubleval            = std::numeric_limits<double>::max() * .999999;

      ins.set("intcol", intval)
         .set("longcol", longval)
         .set("unsignedcol", uval)
         .set("unsignedlongcol", ulongval)
         .set("int32col", int32val)
         .set("uint32col", uint32val)
         .set("int64col", int64val)
         .set("uint64col", uint64val)
         .set("floatcol", floatval)
         .set("doublecol", doubleval)
         .execute();

      int intres;
      long longres;
      unsigned ures;
      unsigned long ulongres;
      int32_t int32res;
      uint32_t uint32res;
      int64_t int64res;
      uint64_t uint64res;
      float floatres;
      double doubleres;

      tntdb::Statement sel = conn.prepare(
        "select"
        "    intcol, longcol, unsignedcol, unsignedlongcol,"
        "    int32col, uint32col, int64col, uint64col,"
        "    floatcol, doublecol"
        " from tntdbtest");

      tntdb::Row row = sel.selectRow();
      row[0].get(intres);
      row[1].get(longres);
      row[2].get(ures);
      row[3].get(ulongres);
      row[4].get(int32res);
      row[5].get(uint32res);
      row[6].get(int64res);
      row[7].get(uint64res);
      row[8].get(floatres);
      row[9].get(doubleres);

      float fq = floatval / floatres;
      double dq = doubleval / doubleres;

      CXXTOOLS_UNIT_ASSERT_EQUALS(intval, intres);
      CXXTOOLS_UNIT_ASSERT_EQUALS(longval, longres);
      CXXTOOLS_UNIT_ASSERT_EQUALS(uval, ures);
      CXXTOOLS_UNIT_ASSERT_EQUALS(ulongval, ulongres);
      CXXTOOLS_UNIT_ASSERT_EQUALS(int32val, int32res);
      CXXTOOLS_UNIT_ASSERT_EQUALS(uint32val, uint32res);
      CXXTOOLS_UNIT_ASSERT_EQUALS(int64val, int64res);
      CXXTOOLS_UNIT_ASSERT_EQUALS(uint64val, uint64res);
      CXXTOOLS_UNIT_ASSERT(fq >= .9999 && fq <= 1.0001);
      CXXTOOLS_UNIT_ASSERT(dq >= .9999 && dq <= 1.0001);
    }

    void testNull()
    {
      tntdb::Statement ins = conn.prepare(
        "insert into tntdbtest(intcol, longcol)"
        " values(:intcol, :longcol)");

      long longval = 43;

      ins.setNull("intcol")
         .set("longcol", longval)
         .execute();

      int intres;
      long longres;

      tntdb::Statement sel = conn.prepare(
        "select intcol, longcol"
        " from tntdbtest");

      tntdb::Row row = sel.selectRow();
      bool intnotnull = row[0].get(intres);
      bool longnotnull = row[1].get(longres);
      CXXTOOLS_UNIT_ASSERT(!intnotnull);
      CXXTOOLS_UNIT_ASSERT(longnotnull);
    }

    void testFloatNan()
    {
      BEGIN_TEST(float, "floatcol");
      float n = std::numeric_limits<float>::quiet_NaN();
      del.execute();                                 \
      ins.set("floatcol", n).execute();
      dbvalue = sel.selectValue();
      dbvalue.get(res);
      CXXTOOLS_UNIT_ASSERT(res != res);
    }

    void testDoubleNan()
    {
      BEGIN_TEST(double, "doublecol");
      double n = std::numeric_limits<double>::quiet_NaN();
      del.execute();                                 \
      ins.set("doublecol", n).execute();
      dbvalue = sel.selectValue();
      dbvalue.get(res);
      CXXTOOLS_UNIT_ASSERT(res != res);
    }

};

cxxtools::unit::RegisterTest<TntdbTypesTest> register_TntdbTypesTest;
