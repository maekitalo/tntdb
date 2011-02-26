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
