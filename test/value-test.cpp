/*
 * Copyright (C) 2018 Tommi Maekitalo
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
#include <tntdb/value.h>
#include <tntdb/impl/value.h>
#include <tntdb/cxxtools/timespan.h>

class ValueTest : public cxxtools::unit::TestSuite
{
    public:
        ValueTest()
            : cxxtools::unit::TestSuite("value")
        {
            registerMethod("testReadValue", *this, &ValueTest::testReadValue);
            registerMethod("testTimespan", *this, &ValueTest::testTimespan);
        }

        void testReadValue()
        {
            tntdb::Value v;

            {
                v = tntdb::Value(new tntdb::ValueImpl("true"));
                bool r = false;
                v.get(r);

                CXXTOOLS_UNIT_ASSERT_EQUALS(r, true);
            }
            {
                v = tntdb::Value(new tntdb::ValueImpl("42"));
                int r = 3;
                v.get(r);

                CXXTOOLS_UNIT_ASSERT_EQUALS(r, 42);
            }
        }

        void testTimespan()
        {
            tntdb::Value v;

            {
                v = tntdb::Value(new tntdb::ValueImpl("15"));
                cxxtools::Microseconds r;
                v.get(r);

                CXXTOOLS_UNIT_ASSERT_EQUALS(r, cxxtools::Microseconds(15));
            }
            {
                v = tntdb::Value(new tntdb::ValueImpl("15"));
                cxxtools::Milliseconds r;
                v.get(r);

                CXXTOOLS_UNIT_ASSERT_EQUALS(r, cxxtools::Milliseconds(15));
            }
            {
                v = tntdb::Value(new tntdb::ValueImpl("15"));
                cxxtools::Seconds r;
                v.get(r);

                CXXTOOLS_UNIT_ASSERT_EQUALS(r, cxxtools::Seconds(15));
            }
            {
                v = tntdb::Value(new tntdb::ValueImpl("15"));
                cxxtools::Minutes r;
                v.get(r);

                CXXTOOLS_UNIT_ASSERT_EQUALS(r, cxxtools::Minutes(15));
            }
            {
                v = tntdb::Value(new tntdb::ValueImpl("15"));
                cxxtools::Hours r;
                v.get(r);

                CXXTOOLS_UNIT_ASSERT_EQUALS(r, cxxtools::Hours(15));
            }
            {
                v = tntdb::Value(new tntdb::ValueImpl("15"));
                cxxtools::Days r;
                v.get(r);

                CXXTOOLS_UNIT_ASSERT_EQUALS(r, cxxtools::Days(15));
            }
        }
};

cxxtools::unit::RegisterTest<ValueTest> register_ValueTest;
