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
#include <cxxtools/convert.h>
#include <tntdb/statement.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <tntdb/cxxtools/timespan.h>

class TestStmt : public tntdb::IStatement
{
        std::map<std::string, std::string> _values;

    public:
        virtual void clear() { _values.clear(); }

        virtual void setNull(const std::string& col) { _values.erase(col); }
        virtual void setBool(const std::string& col, bool data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setShort(const std::string& col, short data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setInt(const std::string& col, int data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setLong(const std::string& col, long data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setUnsignedShort(const std::string& col, unsigned short data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setUnsigned(const std::string& col, unsigned data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setUnsignedLong(const std::string& col, unsigned long data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setInt32(const std::string& col, int32_t data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setUnsigned32(const std::string& col, uint32_t data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setInt64(const std::string& col, int64_t data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setUnsigned64(const std::string& col, uint64_t data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setDecimal(const std::string& col, const tntdb::Decimal& data)   { _values[col] = data.toString(); }
        virtual void setFloat(const std::string& col, float data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setDouble(const std::string& col, double data)   { _values[col] = cxxtools::convert<std::string>(data); }
        virtual void setChar(const std::string& col, char data)   { _values[col] = data; }
        virtual void setString(const std::string& col, const std::string& data)   { _values[col] = data; }
        virtual void setBlob(const std::string& col, const tntdb::Blob& data)   { _values[col] = std::string(data.data(), data.size()); }
        virtual void setDate(const std::string& col, const tntdb::Date& data)   { _values[col] = data.getIso(); }
        virtual void setTime(const std::string& col, const tntdb::Time& data)   { _values[col] = data.getIso(); }
        virtual void setDatetime(const std::string& col, const tntdb::Datetime& data)   { _values[col] = data.getIso(); }

        virtual size_type execute()   { return 0; }
        virtual tntdb::Result select()   { return tntdb::Result(); }
        virtual tntdb::Row selectRow()   { return tntdb::Row(); }
        virtual tntdb::Value selectValue()   { return tntdb::Value(); }
        virtual tntdb::ICursor* createCursor(unsigned fetchsize)   { return 0; }

        std::string value(const std::string& col) const
        {
            std::map<std::string, std::string>::const_iterator it = _values.find(col);
            return it == _values.end() ? std::string() : it->second;
        }

        bool isNull(const std::string& col) const
        { return _values.find(col) == _values.end(); }
};

class StatementTest : public cxxtools::unit::TestSuite
{
    public:
        StatementTest()
            : cxxtools::unit::TestSuite("statement")
        {
            registerMethod("testSetValue", *this, &StatementTest::testSetValue);
            registerMethod("testClear", *this, &StatementTest::testClear);
            registerMethod("testSetTimespan", *this, &StatementTest::testSetTimespan);
        }

        void testSetValue()
        {
            TestStmt* testStmt = new TestStmt();
            tntdb::Statement stmt(testStmt);
            stmt.set("bool", true)
                .set("short", short(42))
                .set("int", int(42))
                .set("long", long(45l))
                .set("unsignedshort", (unsigned short)33)
                .set("unsigned", (unsigned)45)
                .set("unsignedlong", (unsigned long)46)
                .set("int32", (int32_t)50)
                .set("unsigned32", (uint32_t)51)
                .set("int64", (int64_t)52)
                .set("unsigned64", (uint64_t)53)
                .set("decimal", tntdb::Decimal("45.456"))
                .set("float", (float)54)
                .set("double", (double)55)
                .set("char", 'X')
                .set("string", "Hi there")
                .set("ustring", cxxtools::String("M\xc3\xa4kitalo"))
                .set("date", tntdb::Date(2018, 5, 3))
                .set("time", tntdb::Time(13, 21, 11))
                .set("datetime", tntdb::Datetime(2017, 9, 29, 13, 33, 41));
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("bool"), "true");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("short"), "42");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("int"), "42");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("long"), "45");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("unsignedshort"), "33");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("unsigned"), "45");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("unsignedlong"), "46");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("int32"), "50");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("unsigned32"), "51");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("int64"), "52");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("unsigned64"), "53");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("decimal"), "45.456");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("float"), "54");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("double"), "55");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("char"), "X");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("string"), "Hi there");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("ustring"), "M\xc3\xa4kitalo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("date"), "2018-05-03");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("time"), "13:21:11.000");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("datetime"), "2017-09-29 13:33:41.000");
        }

        void testClear()
        {
            TestStmt* testStmt = new TestStmt();
            tntdb::Statement stmt(testStmt);
            stmt.set("bool", true)
                .set("int", short(42));

            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("bool"), "true");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("int"), "42");

            stmt.clear();

            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("bool"), "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("int"), "");
        }

        void testSetTimespan()
        {
            TestStmt* testStmt = new TestStmt();
            tntdb::Statement stmt(testStmt);
            stmt.set("us", cxxtools::Microseconds(17))
                .set("ms", cxxtools::Milliseconds(18))
                .set("s", cxxtools::Seconds(19.01234))
                .set("m", cxxtools::Minutes(20))
                .set("h", cxxtools::Hours(21))
                .set("d", cxxtools::Days(22));

            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("us"), "17");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("ms"), "18");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("s"), "19.01234");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("m"), "20");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("h"), "21");
            CXXTOOLS_UNIT_ASSERT_EQUALS(testStmt->value("d"), "22");
        }

};

cxxtools::unit::RegisterTest<StatementTest> register_StatementTest;
