/*
 * Copyright (C) 2014 Tommi Maekitalo
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


/*

 This demonstrates the various ways to read data from a database with tntdb.

 */

#include <iostream>
#include <cxxtools/log.h>
#include <tntdb/connect.h>
#include <tntdb/statement.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);  // logging should be initialized always

        if (argc < 2)
        {
            std::cerr << "Usage: " << argv[0] << " dburl" << std::endl;
            return -1;
        }

        const std::string database = argv[1];
        auto db = tntdb::connect(database);

        // selecting a single value
        //
        // tntdb::Connection::selectValue executes the statement and selects the
        // first value of the first row as a tntdb::Value
        //
        // tntdb::Value::get(T&) extracts the value and fills the passed variable
        // It returns a bool whether the value was not null (which is ignored here)
        unsigned count;
        db.selectValue("select count(*)"
                       "  from foo")      // returns a tntdb::Value
          .get(count);                    // extracts unsigned value from tntdb::Value

        // selecting multiple values of a row
        //
        // tntdb::Statement::selectRow() selects the first row as tntdb::Row.
        // tntdb::Row::get(T&) extracts the value and returns a tntdb::RowReader,
        // tntdb::RowReader::get(T&) extracts the current column, increases the
        // column counter by 1 and returns the row reader.
        unsigned max, min;
        db.selectRow("select max(a), min(a) from foo")
          .get(max)
          .get(min);

        // reading a result set
        // Note that a tntdb::Result helds the whole result in memory, which may not be what you want.
        // Use tntdb::Statement to fetch step by step.
        auto result = db.select("select a, b from foo");
        for (auto row: result)
        {
            unsigned a;
            std::string b;

            row.get(a)
               .get(b);

            // if a or b may be null:
            bool a_isnull, b_isnull;
            row.get(a, a_isnull)
               .get(b, b_isnull);

            // or using std::optional which handles nullable values nicely
            {
                std::optional<unsigned> a;
                std::optional<std::string> b;
                row.get(a)
                   .get(b);
            }
        }

        // better than a tntdb::Result is to use a cursor
        // Iterating a statement creates a cursor over the results.
        // The result is not held in memory in this case.
        auto stmt = db.prepare(R"SQL("
          select a, b
            from foo"
          where a > :a
        )SQL");
        stmt.set("a", 42);

        for (auto row: stmt)
        {
            unsigned a;
            std::string b;

            row.get(a)
               .get(b);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

