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

#ifndef TNTDB_SQLBUILDER_H
#define TNTDB_SQLBUILDER_H

#include <string>

namespace tntdb
{
  /** Helper class for building sql statements.

      Sql statments take placeholders, which can be filled with values for
      execution. Those placeholders are limited to take scalar values and only
      where scalar values are expected in the sql statement.

      Sometimes the user needs to pass sets of values. Since it is not directly
      supported by sql the user has to build a list of placeholders into the
      sql statement. If the number of needed placeholders is not known at
      compile time, a sql statement needs to be created dynamically.

      Anoter problem is, that placeholders are not allowed everywhere in the
      sql statment. To parametrise e.g. the table name, again the user has to
      replace the table name before passing the sql to the database.

      This class has helpers for both cases.

      Placeholders here start with a percent character followed by alphanumeric
      characters.

   */
  class SqlBuilder
  {
    public:
      /// The class is default constructable.
      SqlBuilder()
        { }

      /// Create a sql builder with a string with optional placeholders.
      SqlBuilder(const std::string& sql_)
        : sql(sql_)
        { }

      /** Replace a sqlbuilder placeholder with a list of sql placeholders with index.

          The varname specifies the placeholder and the count the number of
          generated placeholders.

          example:

          @code
            std::set<int> myValues;
            myValues.insert(2);
            myValues.insert(3);
            myValues.insert(6);

            tntdb::Statement st = conn.prepare(
                tntdb::SqlBuilder(
                    "select foo from bar where baz in (%set)")
                        .extendParam("set", myValues.size())
                );

            st.set("set", s);
            // => select foo from bar where baz in (:set1,:set2,:set3)
          @endcode

          This generates a sql statement with 3 sql placeholders and fills
          those with values from a std::set.

       */
      SqlBuilder& extendParam(const std::string& varname, unsigned count);

      /** Replace a placeholder with a value.

          This method replaces a sqlbuilder placeholder with a value.
          Note that no escaping is done so that passing user input may result
          in sql injection.

          example:

          @code
            std::string myTablename = "bar";
            tntdb::Statement st = conn.prepare(
                tntdb::SqlBuilder(
                    "select foo from %tab")
                        .replace("tab", myTablename)
                );
            // => select foo from bar
          @endcode

       */
      SqlBuilder& replace(const std::string& varname, const std::string& value);

      /** Replace a placeholder conditionally with a value.

          This adds a condition to the replace method. This helps when optional
          conditions are needed.

          @code

            std::string searchString = "";
            tntdb::Statement st1 = conn.prepare(
                tntdb::SqlBuilder(
                    "select foo from mytable where bar = 17 %cond"
                        .replaceIf(!searchString.empty(), "s", "and bar like %s")
                );
            // => select foo from mytable where bar = 17

            searchString = "foo%";
            tntdb::Statement st2 = conn.prepare(
                tntdb::SqlBuilder(
                    "select foo from mytable where bar = 17 %cond"
                        .replaceIf(!searchString.empty(), "s", "and bar like :s")
                );
            // => select foo from mytable where bar = 17 and bar like :s

          @endcode
       */
      SqlBuilder& replaceIf(bool condition, const std::string& varname, const std::string& value,
                            const std::string& elsevalue = std::string())
      {
        replace(varname, condition ? value : elsevalue);
        return *this;
      }

      /// sets the underlying string to the passed value
      void str(const std::string& s)      { sql = s; }

      /// returns the resulting sql string.
      const std::string& str() const      { return sql; }

      /// returns the resulting sql string implicitly.
      operator const std::string&() const { return sql; }

    private:
      std::string sql;
  };
}

#endif // TNTDB_SQLBUILDER_H

