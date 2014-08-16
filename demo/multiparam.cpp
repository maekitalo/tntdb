/*
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
#include <iostream>
#include <tntdb/connect.h>
#include <tntdb/statement.h>
#include <tntdb/sqlbuilder.h>
#include <tntdb/row.h>
#include <cxxtools/log.h>
#include <vector>
#include <set>

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    tntdb::Connection conn = tntdb::connectCached("sqlite:mydb.db");

    std::vector<int> v;
    v.push_back(5);
    v.push_back(2);

    std::set<double> w;

    tntdb::Statement stmt = conn.prepare(
                                tntdb::SqlBuilder("select a from %table where a in (%v) or b in (%w) and z = :z %q1 %q2")
                                    .extendParam("v", v.size())
                                    .extendParam("w", w.size())
                                    .replace("table", mytable)
                                    .replaceIf(condition1, "q1", "and z > :z")
                                    .replaceIf(condition2, "q2", "and y > :y")
                                    );

    stmt.set("v", v);
    stmt.set("w", w);
    if (condition1)
      stmt.set("z", 5);
    if (condition2)
      stmt.set("y", 18);

    for (tntdb::Statement::const_iterator cur = stmt.begin(); cur != stmt.end(); ++cur)
    {
      int vv;
      (*cur)[0].get(vv);
      std::cout << vv << '\t';
    }
    std::cout << std::endl;

  }
  catch (const std::exception& e)
  {
    std::cerr << "failed: " << e.what() << std::endl;
  }
}

