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
#include <tntdb/decimal.h>
#include <limits>
#include <math.h>

namespace
{
  bool nearBy(long double v1, long double v2, long double e = 1e-6)
  {
    long double q = v1 / v2;
    return q > 1 - e && q < 1 + e;
  }
}

class TntdbDecimalTest : public cxxtools::unit::TestSuite
{
  public:
    TntdbDecimalTest()
      : cxxtools::unit::TestSuite("decimal")
    {
      registerMethod("testDouble", *this, &TntdbDecimalTest::testDouble);
      registerMethod("testFromString", *this, &TntdbDecimalTest::testFromString);
      registerMethod("testToString", *this, &TntdbDecimalTest::testToString);
      registerMethod("testInt", *this, &TntdbDecimalTest::testInt);
      registerMethod("testCompare", *this, &TntdbDecimalTest::testCompare);
    }

    void testDouble()
    {
      tntdb::Decimal d;

      d = tntdb::Decimal(45.6);
      CXXTOOLS_UNIT_ASSERT(nearBy(d.getDouble(), 45.6));

      d = tntdb::Decimal(-3.14);
      CXXTOOLS_UNIT_ASSERT(nearBy(d.getDouble(), -3.14));

      d = tntdb::Decimal(5e97);
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), 5e97);

      d = tntdb::Decimal(-123e-154);
      CXXTOOLS_UNIT_ASSERT(nearBy(d.getDouble(), -123e-154));

      d = tntdb::Decimal(std::numeric_limits<double>::max());
      CXXTOOLS_UNIT_ASSERT(nearBy(d.getDouble(), std::numeric_limits<double>::max()));

      d = tntdb::Decimal(std::numeric_limits<double>::min());
      CXXTOOLS_UNIT_ASSERT(nearBy(d.getDouble(), std::numeric_limits<double>::min()));

      d = tntdb::Decimal(std::numeric_limits<double>::infinity());
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), std::numeric_limits<double>::infinity());

      d = tntdb::Decimal(-std::numeric_limits<double>::infinity());
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), -std::numeric_limits<double>::infinity());

    }

    void testFromString()
    {
      tntdb::Decimal d;

      d = tntdb::Decimal("45.6");
      CXXTOOLS_UNIT_ASSERT(nearBy(d.getDouble(), 45.6));

      d = tntdb::Decimal("-3.14");
      CXXTOOLS_UNIT_ASSERT(nearBy(d.getDouble(), -3.14));

      d = tntdb::Decimal("5e97");
      CXXTOOLS_UNIT_ASSERT(nearBy(d.getDouble(), 5e97));

      d = tntdb::Decimal("-123e-154");
      CXXTOOLS_UNIT_ASSERT(nearBy(d.getDouble(), -123e-154));

      d = tntdb::Decimal("12345678901234567890");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toStringFix(), "12345678901234567890");

      d = tntdb::Decimal("inf");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), std::numeric_limits<double>::infinity());

      d = tntdb::Decimal("-inf");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), -std::numeric_limits<double>::infinity());

      d = tntdb::Decimal("nan");
      CXXTOOLS_UNIT_ASSERT(d.getDouble() != d.getDouble());

      d = tntdb::Decimal("  1.00000000000000000000000000000000000000E+00  ");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), 1);

      d = tntdb::Decimal("  1  ");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), 1);

      d = tntdb::Decimal("  1.0  ");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), 1);

      d = tntdb::Decimal("  1e0  ");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), 1);

      d = tntdb::Decimal("  0.1e1  ");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), 1);

      d = tntdb::Decimal("  .00000000000000000000000000000000000000E+00");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), 0);

      d = tntdb::Decimal("0");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.getDouble(), 0);

    }

    void testToString()
    {
      tntdb::Decimal d;

      d = tntdb::Decimal("45.6");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toString(), "45.6");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toStringSci(), "4.56e1");

      d = tntdb::Decimal("-3.14");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toString(), "-3.14");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toStringSci(), "-3.14e0");

      d = tntdb::Decimal("5e97");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toString(), "5e97");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toStringSci(), "5e97");

      d = tntdb::Decimal("-123e-154");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toString(), "-1.23e-152");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toStringSci(), "-1.23e-152");

      d = tntdb::Decimal("12345678901234567890");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toStringFix(), "12345678901234567890");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toStringSci(), "1.234567890123456789e19");

      d = tntdb::Decimal("1e23");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toStringFix(), "100000000000000000000000");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toStringSci(), "1e23");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toString(), "1e23");

      d = tntdb::Decimal("-3e-23");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toStringFix(), "-0.00000000000000000000003");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toStringSci(), "-3e-23");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toString(), "-3e-23");

      d = tntdb::Decimal("inf");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toString(), "inf");

      d = tntdb::Decimal("-inf");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toString(), "-inf");

      d = tntdb::Decimal("nan");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.toString(), "nan");

    }

    void testInt()
    {
      tntdb::Decimal d;
      long l;

      d = tntdb::Decimal(100, 0);
      l = d.getInteger<long>();
      CXXTOOLS_UNIT_ASSERT_EQUALS(l, 100l);

      d = tntdb::Decimal(std::numeric_limits<long>::max(), 0);
      l = d.getInteger<long>();
      CXXTOOLS_UNIT_ASSERT_EQUALS(l, std::numeric_limits<long>::max());

      d = tntdb::Decimal(std::numeric_limits<long>::min(), 0);
      l = d.getInteger<long>();
      CXXTOOLS_UNIT_ASSERT_EQUALS(l, std::numeric_limits<long>::min());

      d = tntdb::Decimal(-100, 0);
      l = d.getInteger<long>();
      CXXTOOLS_UNIT_ASSERT_EQUALS(l, -100l);

    }

    void testCompare()
    {
      tntdb::Decimal d0(45, 3);
      tntdb::Decimal d1(450, 2);
      tntdb::Decimal d2(449, 2);
      tntdb::Decimal d3(-449, 2);
      tntdb::Decimal d4(451, 2);

      CXXTOOLS_UNIT_ASSERT(d0 == d1);

      CXXTOOLS_UNIT_ASSERT(d0 != d2);
      CXXTOOLS_UNIT_ASSERT(d1 != d2);
      CXXTOOLS_UNIT_ASSERT(d3 != d0);
      CXXTOOLS_UNIT_ASSERT(d3 != d1);
      CXXTOOLS_UNIT_ASSERT(d3 != d2);

      CXXTOOLS_UNIT_ASSERT(d2 < d0);
      CXXTOOLS_UNIT_ASSERT(d2 < d1);
      CXXTOOLS_UNIT_ASSERT(d3 < d0);
      CXXTOOLS_UNIT_ASSERT(d3 < d1);
      CXXTOOLS_UNIT_ASSERT(d3 < d2);
      CXXTOOLS_UNIT_ASSERT(d2 < d4);

      CXXTOOLS_UNIT_ASSERT(d0 > d2);
      CXXTOOLS_UNIT_ASSERT(d1 > d2);
      CXXTOOLS_UNIT_ASSERT(d0 > d3);
      CXXTOOLS_UNIT_ASSERT(d1 > d3);
      CXXTOOLS_UNIT_ASSERT(d2 > d3);

      CXXTOOLS_UNIT_ASSERT(d2 <= d0);
      CXXTOOLS_UNIT_ASSERT(d0 <= d1);
      CXXTOOLS_UNIT_ASSERT(d2 <= d1);

      CXXTOOLS_UNIT_ASSERT(d0 >= d2);
      CXXTOOLS_UNIT_ASSERT(d0 >= d1);
      CXXTOOLS_UNIT_ASSERT(d1 >= d2);
    }

};

cxxtools::unit::RegisterTest<TntdbDecimalTest> register_TntdbDecimalTest;
