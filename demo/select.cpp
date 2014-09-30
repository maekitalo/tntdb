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
    log_init();  // logging should be initialized always

    if (argc < 2)
    {
      std::cerr << "Usage: " << argv[0] << " dburl" << std::endl;
      return -1;
    }

    const std::string database = argv[1];
    tntdb::Connection db = tntdb::connect(database);

    // selecting a single value
    unsigned count;
    db.selectValue("select count(*)"
                   "  from foo")      // returns a tntdb::Value
      .get(count);                    // extracts unsigned value from tntdb::Value

    // selecting multiple values
    unsigned max, min;
    tntdb::Row row = db.selectRow("select max(a) as max, min(a) as min"
                                  "  from foo");
    row["max"].get(max);
    row["min"].get(min);

    // reading multiple values from a row with tntdb::RowReader
    db.selectRow("select max(a), min(a)"
                 "  from foo")            // returns a tntdb::Row
      .get(max)                           // extracts unsigned and returns a tntdb::RowReader
      .get(min);

    // reading a result set
    // Note that a tntdb::Result helds the whole result in memory, which may not be what you want.
    // Use tntdb::Statement to fetch step by step.
    tntdb::Result result = db.select("select a, b"
                                     " from foo");
    for (tntdb::Result::const_iterator it = result.begin(); it != result.end(); ++it)
    {
      tntdb::Row r = *it;  // dereferencing returns a tntdb::Row
      unsigned a;
      std::string b;

      bool a_isnull = r["a"].get(a);  // get returns a boolean wether the value was null
      bool b_isnull = r["b"].get(b);

      // as an alternative we can use index
      r[0].get(a);
      r[1].get(b);

      // or again tntdb::RowReader
      r.get(a)
       .get(b);

      // a or b may be null:
      r.get(a, a_isnull)
       .get(b, b_isnull);
    }

    // using tntdb::Statement

    // fetching value
    db.prepare("select count(*)"
               "  from foo"
               " where a > :a")  // returns tntdb::Statement
      .set("a", 42)                                      // sets :a to a value
      .selectValue()                                     // returns a tntdb::Value
      .get(count);                                       // extracts unsigned value from tntdb::Value

    // fetching multiple values
    row = db.prepare("select max(a) as max, min(a) as min"
                     "  from foo"
                     " where a > :a")
            .set("a", 42)
            .selectRow();
    row["max"].get(max);
    row["min"].get(min);

    // reading multiple values from a row with tntdb::RowReader
    db.prepare("select max(a), min(a)"
               "  from foo"
               " where a > :a")
      .set("a", 42)                                      // sets :a to a value
      .selectRow()
      .get(max)                                     // extracts unsigned and returns a tntdb::RowReader
      .get(min);

    // reading data step by step
    tntdb::Statement stmt = db.prepare("select a, b"
                                       "  from foo"
                                       " where a > :a");
    stmt.set("a", 42);
    for (tntdb::Statement::const_iterator it = stmt.begin(); it != stmt.end(); ++it)
    {
      tntdb::Row r = *it;  // dereferencing returns a tntdb::Row
      unsigned a;
      std::string b;

      // the row can be read just like in tntdb::Result
      bool a_isnull = r["a"].get(a);  // get returns a boolean wether the value was null
      bool b_isnull = r["b"].get(b);

      // the iterator can also return a RowReader
      it.get(a, a_isnull)
        .get(b, b_isnull);
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

