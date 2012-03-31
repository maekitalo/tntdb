/*
 * Copyright (C) 2011 Tommi Maekitalo
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
#include <tntdb/sqlbuilder.h>
#include <limits>

class SqlBuilderTest : public cxxtools::unit::TestSuite
{

  public:
    SqlBuilderTest()
      : cxxtools::unit::TestSuite("sqlbuilder")
    {
      registerMethod("testExtendParam", *this, &SqlBuilderTest::testExtendParam);
      registerMethod("testReplace", *this, &SqlBuilderTest::testReplace);
      registerMethod("testReplaceQ", *this, &SqlBuilderTest::testReplaceQ);
      registerMethod("testReplaceIf", *this, &SqlBuilderTest::testReplaceIf);
    }

    void testExtendParam()
    {
      tntdb::SqlBuilder s("select a from tab1 where b in (%c)");
      s.extendParam("c", 3);
      CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "select a from tab1 where b in ( :c0,:c1,:c2 )");
    }

    void testReplace()
    {
      tntdb::SqlBuilder s("select a from %table where b = 5");
      s.replace("table", "foo");
      CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "select a from foo where b = 5");
    }

    void testReplaceQ()
    {
      tntdb::SqlBuilder s("select a from tab1 where b = 'Hi' and %col = 5");
      s.replace("col", "c");
      CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "select a from tab1 where b = 'Hi' and c = 5");

      tntdb::SqlBuilder s2("select a from tab1 where b = '\\' and %col = 5");
      s2.replace("col", "c");
      CXXTOOLS_UNIT_ASSERT_EQUALS(s2.str(), "select a from tab1 where b = '\\' and c = 5");
    }

    void testReplaceIf()
    {
      tntdb::SqlBuilder s("select a from tab1 where b = 5 %c1 %c2");
      s.replaceIf(true, "c1", "and c = :c")
       .replaceIf(false, "c2", "and d = :d");
      CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "select a from tab1 where b = 5 and c = :c ");
    }

};

cxxtools::unit::RegisterTest<SqlBuilderTest> register_SqlBuilderTest;
