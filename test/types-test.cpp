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
#include <cxxtools/log.h>
#include <stdlib.h>
#include <tntdb/connect.h>
#include <tntdb/statement.h>
#include <tntdb/row.h>
#include <limits>

log_define("tntdb.unit.types")

template <typename T>
class TypesTester
{
    std::string _colName;
    tntdb::Statement _del;
    tntdb::Statement _ins;
    tntdb::Statement _sel;

public:
    explicit TypesTester(tntdb::Connection conn, const std::string& colName)
        : _colName(colName),
          _del(conn.prepare("delete from tntdbtest")),
          _ins(conn.prepare("insert into tntdbtest(" + _colName + ") values(:" + _colName + ")")),
          _sel(conn.prepare("select " + _colName + " from tntdbtest"))
    { }

    void test(const T& val)
    {
        _del.execute();
        _ins.set(_colName, val)
            .execute();

        T result;
        bool isNotNull;

        // read using selectValue
        auto dbvalue = _sel.selectValue();
        isNotNull = dbvalue.get(result);
        CXXTOOLS_UNIT_ASSERT(isNotNull);
        CXXTOOLS_UNIT_ASSERT_EQUALS(val, result);

        // read from result
        isNotNull = _sel.select()[0][0].get(result);
        CXXTOOLS_UNIT_ASSERT(isNotNull);
        CXXTOOLS_UNIT_ASSERT_EQUALS(val, result);

        // read using cursor
        auto cursor = _sel.begin(4);
        CXXTOOLS_UNIT_ASSERT(cursor != _sel.end());
        isNotNull = (*cursor)[0].get(result);
        CXXTOOLS_UNIT_ASSERT(isNotNull);
        CXXTOOLS_UNIT_ASSERT_EQUALS(val, result);
    }

    void testNoEq(const T& val)
    {
        _del.execute();
        _ins.set(_colName, val)
            .execute();

        T result;
        bool isNotNull;

        // read using selectValue
        auto dbvalue = _sel.selectValue();
        isNotNull = dbvalue.get(result);
        CXXTOOLS_UNIT_ASSERT(isNotNull);
        CXXTOOLS_UNIT_ASSERT(val == result);

        // read from result
        isNotNull = _sel.select()[0][0].get(result);
        CXXTOOLS_UNIT_ASSERT(isNotNull);
        CXXTOOLS_UNIT_ASSERT(val == result);

        // read using cursor
        auto cursor = _sel.begin(4);
        CXXTOOLS_UNIT_ASSERT(cursor != _sel.end());
        isNotNull = (*cursor)[0].get(result);
        CXXTOOLS_UNIT_ASSERT(isNotNull);
        CXXTOOLS_UNIT_ASSERT(val == result);
    }

    void testFloat(T val)
    {
        _del.execute();
        _ins.set(_colName, val)
            .execute();

        T result;
        bool isNotNull;

        auto dbvalue = _sel.selectValue();
        isNotNull = dbvalue.get(result);
        CXXTOOLS_UNIT_ASSERT(isNotNull);
        CXXTOOLS_UNIT_ASSERT(val / result >= .9999 && val / result <= 1.0001);
        isNotNull = _sel.select()[0][0].get(result);
        CXXTOOLS_UNIT_ASSERT(isNotNull);
        CXXTOOLS_UNIT_ASSERT(val / result >= .9999 && val / result <= 1.0001);
        auto cursor = _sel.begin(4);
        CXXTOOLS_UNIT_ASSERT(cursor != _sel.end());
        isNotNull = (*cursor)[0].get(result);
        CXXTOOLS_UNIT_ASSERT(isNotNull);
        CXXTOOLS_UNIT_ASSERT_EQUALS(val, result);
    }

    void testDt(const T& val)
    {
        _del.execute();
        _ins.set(_colName, val)
            .execute();

        T result;
        bool isNotNull;

        auto dbvalue = _sel.selectValue();
        isNotNull = dbvalue.get(result);
        if (isNotNull)
        {
            CXXTOOLS_UNIT_ASSERT(!result.isNull());
            CXXTOOLS_UNIT_ASSERT_EQUALS(val.getIso(), result.getIso());
        }
        else
            CXXTOOLS_UNIT_ASSERT(val.isNull());

        isNotNull = _sel.select()[0][0].get(result);
        if (isNotNull)
        {
            CXXTOOLS_UNIT_ASSERT(!result.isNull());
            CXXTOOLS_UNIT_ASSERT_EQUALS(val.getIso(), result.getIso());
        }
        else
            CXXTOOLS_UNIT_ASSERT(val.isNull());
    }

    void testNan()
    {
        T val = std::numeric_limits<T>::quiet_NaN();

        _del.execute();
        _ins.set(_colName, val)
            .execute();

        T result;
        bool isNotNull;

        auto dbvalue = _sel.selectValue();
        isNotNull = dbvalue.get(result);
        CXXTOOLS_UNIT_ASSERT(isNotNull);
        CXXTOOLS_UNIT_ASSERT(result != result);
    }
};

class TntdbTypesTest : public cxxtools::unit::TestSuite
{
    tntdb::Connection conn;
    tntdb::Statement del;

public:
    TntdbTypesTest()
      : cxxtools::unit::TestSuite("types")
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
      registerMethod("testFloatFromDecimal", *this, &TntdbTypesTest::testFloatFromDecimal);
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
    }

    void tearDown()
    {
        del.execute();
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

        int intres = 0;
        long longres = 0;
        unsigned ures = 0;
        unsigned long ulongres = 0;
        int32_t int32res = 0;
        uint32_t uint32res = 0;
        int64_t int64res = 0;
        uint64_t uint64res = 0;
        float floatres = 0;
        double doubleres = 0;

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

    void testBool()
    {
        TypesTester<bool> tester(conn, "boolcol");
        tester.test(true);
        tester.test(false);
    }

    void testShort()
    {
        TypesTester<short> tester(conn, "shortcol");
        tester.test(42);
        tester.test(-123);
        tester.test(std::numeric_limits<short>::max());
        tester.test(std::numeric_limits<short>::min());
    }

    void testInt()
    {
        TypesTester<int> tester(conn, "intcol");
        tester.test(42);
        tester.test(-123);
        tester.test(std::numeric_limits<int>::max());
        tester.test(std::numeric_limits<int>::min());
    }

    void testLong()
    {
        TypesTester<long> tester(conn, "longcol");
        tester.test(42);
        tester.test(-123);
        tester.test(std::numeric_limits<long>::max());
        tester.test(std::numeric_limits<long>::min());
    }

    void testUnsignedShort()
    {
        TypesTester<unsigned short> tester(conn, "unsignedshortcol");
        tester.test(42);
        tester.test(std::numeric_limits<unsigned short>::max());
        tester.test(std::numeric_limits<unsigned short>::min());
    }

    void testUnsigned()
    {
        TypesTester<unsigned> tester(conn, "unsignedcol");
        tester.test(42);
        tester.test(std::numeric_limits<unsigned>::max());
        tester.test(std::numeric_limits<unsigned>::min());
    }

    void testUnsignedLong()
    {
        TypesTester<unsigned long> tester(conn, "unsignedlongcol");
        tester.test(42);
        tester.test(std::numeric_limits<unsigned long>::max());
        tester.test(std::numeric_limits<unsigned long>::min());
    }

    void testInt32()
    {
        TypesTester<int32_t> tester(conn, "int32col");
        tester.test(42);
        tester.test(-123);
        tester.test(std::numeric_limits<int32_t>::max());
        tester.test(std::numeric_limits<int32_t>::min());
    }

    void testUint32()
    {
        TypesTester<uint32_t> tester(conn, "uint32col");
        tester.test(42);
        tester.test(std::numeric_limits<uint32_t>::max());
        tester.test(std::numeric_limits<uint32_t>::min());
    }

    void testInt64()
    {
        TypesTester<int64_t> tester(conn, "int64col");
        tester.test(42);
        tester.test(-123);
        tester.test(std::numeric_limits<int64_t>::max());
        tester.test(std::numeric_limits<int64_t>::min());
    }

    void testUint64()
    {
        TypesTester<uint64_t> tester(conn, "uint64col");
        tester.test(42);
        tester.test(std::numeric_limits<uint64_t>::max());
        tester.test(std::numeric_limits<uint64_t>::min());
    }

    void testDecimal()
    {
        TypesTester<tntdb::Decimal> tester(conn, "decimalcol");
        tester.test(tntdb::Decimal(456, 8));
        tester.test(tntdb::Decimal(456, -4));
        tester.test(tntdb::Decimal(-456, 8));
    }

    void testFloat()
    {
        TypesTester<float> tester(conn, "floatcol");
        tester.testFloat(static_cast<float>(42));
        tester.testFloat(static_cast<float>(-123));
        tester.testFloat(std::numeric_limits<float>::max() * .999999);
        tester.testFloat(std::numeric_limits<float>::min() * 1.00001);
        tester.test(std::numeric_limits<float>::infinity());
        tester.test(-std::numeric_limits<float>::infinity());
    }

    void testFloatFromDecimal()
    {
        TypesTester<float> tester(conn, "floatcol");
        tester.testFloat(static_cast<float>(42));
        tester.testFloat(static_cast<float>(-123));
    }

    void testDouble()
    {
        TypesTester<double> tester(conn, "doublecol");
        tester.testFloat(static_cast<double>(42));
        tester.testFloat(static_cast<double>(-123));
        tester.testFloat(std::numeric_limits<double>::max() * .999999);
        tester.testFloat(std::numeric_limits<double>::min() * 1.00001);
        tester.test(std::numeric_limits<double>::infinity());
        tester.test(-std::numeric_limits<double>::infinity());
    }

    void testChar()
    {
        TypesTester<char> tester(conn, "charcol");
        tester.test('A');
        tester.test('Z');
        tester.test('\n');
    }

    void testString()
    {
        TypesTester<std::string> tester(conn, "stringcol");
        tester.test(std::string("Hello\n"));
        tester.test(std::string("M\xc3\xa4kitalo"));
    }

    void testBlob()
    {
        TypesTester<tntdb::Blob> tester(conn, "blobcol");
        tester.testNoEq(tntdb::Blob("\0\1\2\3\0xff", 5));
    }

    void testDate()
    {
        TypesTester<tntdb::Date> tester(conn, "datecol");
        tester.testDt(tntdb::Date(2010, 2, 15));
        tester.testDt(tntdb::Date());
    }

    void testTime()
    {
        TypesTester<tntdb::Time> tester(conn, "timecol");
        tester.testDt(tntdb::Time(20, 9, 31, 12));
        tester.testDt(tntdb::Time());
    }

    void testDatetime()
    {
        TypesTester<tntdb::Datetime> tester(conn, "datetimecol");
        tester.testDt(tntdb::Datetime(2010, 2, 15, 20, 9, 31, 12));
        tester.testDt(tntdb::Datetime());
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

    void testFloatNan()
    {
        TypesTester<float> tester(conn, "floatcol");
        tester.testNan();
    }

    void testDoubleNan()
    {
        TypesTester<double> tester(conn, "doublecol");
        tester.testNan();
    }

};

cxxtools::unit::RegisterTest<TntdbTypesTest> register_TntdbTypesTest;
