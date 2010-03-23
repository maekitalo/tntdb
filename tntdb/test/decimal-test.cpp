/*
 * Copyright (C) 2010 Tommi Maekitalo
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


#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>
#include <tntdb/decimal.h>

class TntdbDecimalTest : public cxxtools::unit::TestSuite
{
  public:
    TntdbDecimalTest()
      : cxxtools::unit::TestSuite("tntdb-decimal-Test")
    {
      registerMethod("testSetLong", *this, &TntdbDecimalTest::testSetLong);
      registerMethod("testCompare", *this, &TntdbDecimalTest::testCompare);
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

    void testSetLong()
    {
      tntdb::Decimal d;
      d.setLong(100);
      long l = d.getLong();

      CXXTOOLS_UNIT_ASSERT_EQUALS(l, 100);
    }

};

cxxtools::unit::RegisterTest<TntdbDecimalTest> register_TntdbDecimalTest;
