/*
 * Copyright (C) 2023 Tommi Maekitalo
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
#include <tntdb/row.h>
#include <tntdb/cxxtools/json.h>

log_define("tntdb.unit.types")

namespace
{
class TestObject
{
    friend void operator <<= (cxxtools::SerializationInfo& si, const TestObject& o);
    friend void operator >>= (const cxxtools::SerializationInfo& si, TestObject& o);

    int _value = 0;
    std::string _svalue;

public:
    TestObject() = default;
    TestObject(int value, const std::string& svalue)
        : _value(value),
          _svalue(svalue)
          { }

    int value() const                   { return _value; }
    const std::string& svalue() const   { return _svalue; }
};

void operator <<= (cxxtools::SerializationInfo& si, const TestObject& o)
{
    si.addMember("value") <<= o._value;
    si.addMember("svalue") <<= o._svalue;
}

void operator >>= (const cxxtools::SerializationInfo& si, TestObject& o)
{
    si.getMember("value") >>= o._value;
    si.getMember("svalue") >>= o._svalue;
}

}

class JsonTest : public TntdbTestBase
{
public:
    JsonTest()
      : TntdbTestBase("json")
    {
        registerMethod("testInOUt", *this, &JsonTest::testInOut);
    }

    void testInOut()
    {
        tntdb::Statement ins = conn.prepare(R"SQL(
          insert into tntdbtest(stringcol)
           values(:stringcol)
          )SQL");

        tntdb::Statement sel = conn.prepare(R"SQL(
          select stringcol from tntdbtest
          )SQL");

        TestObject in(42, "Hi there");
        TestObject out;

        ins.set("stringcol", cxxtools::Json(in))
           .execute();

        sel.selectValue()
           .get(cxxtools::Json(out));

        CXXTOOLS_UNIT_ASSERT_EQUALS(out.value(), 42);
        CXXTOOLS_UNIT_ASSERT_EQUALS(out.svalue(), "Hi there");
    }
};

cxxtools::unit::RegisterTest<JsonTest> register_JsonTest;
