/*
 * Copyright (C) 2016 Tommi Maekitalo
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


#include "testbase.h"
#include <cxxtools/unit/registertest.h>
#include <cxxtools/log.h>
#include <tntdb/statement.h>
#include <tntdb/value.h>
#include <tntdb/cxxtools/timespan.h>

log_define("tntdb.unit.timespan")

class TntdbTimespanTest : public TntdbTestBase
{
public:
    TntdbTimespanTest()
      : TntdbTestBase("timespan")
    {
        registerMethod("testTimespan", *this, &TntdbTimespanTest::testTimespan);
    }

    void testTimespan()
    {
        tntdb::Statement ins = conn.prepare(
            "insert into tntdbtest(intcol, doublecol, unsignedcol) values(:ts1, :ts2, :ts3)");
        ins.set("ts1", cxxtools::Milliseconds(2000))
           .set("ts2", cxxtools::Seconds(3.5))
           .set("ts3", cxxtools::Days(2))
           .execute();

        tntdb::Statement sel = conn.prepare(
            "select intcol, doublecol, unsignedcol from tntdbtest");
        tntdb::Row row = sel.selectRow();

        cxxtools::Milliseconds ts1;
        cxxtools::Seconds ts2;
        cxxtools::Days ts3;

        row.get(ts1)
           .get(ts2)
           .get(ts3);

        CXXTOOLS_UNIT_ASSERT_EQUALS(ts1, cxxtools::Milliseconds(2000));
        CXXTOOLS_UNIT_ASSERT_EQUALS(ts2, cxxtools::Seconds(3.5));
        CXXTOOLS_UNIT_ASSERT_EQUALS(ts3, cxxtools::Days(2));

        int v1;
        double v2;
        unsigned v3;

        row.get(v1)
           .get(v2)
           .get(v3);

        CXXTOOLS_UNIT_ASSERT_EQUALS(v1, 2000);
        CXXTOOLS_UNIT_ASSERT_EQUALS(v2, 3.5);
        CXXTOOLS_UNIT_ASSERT_EQUALS(v3, 2);

    }

};

cxxtools::unit::RegisterTest<TntdbTimespanTest> register_TntdbTimespanTest;
