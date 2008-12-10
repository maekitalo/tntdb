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
#include <tntdb/connect.h>
#include <tntdb/result.h>
#include <tntdb/value.h>
#include <cxxtools/arg.h>
#include <cxxtools/timespan.h>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> prepared(argc, argv, 'p');
    cxxtools::Arg<bool> cached(argc, argv, 'c');

    tntdb::Connection conn = tntdb::connect("mysql:db=DS2;user=web;passwd=web");
    unsigned max = conn.selectValue("select max(PROD_ID) from PRODUCTS")
                       .getUnsigned();

    cxxtools::Timespan t0 = cxxtools::Timespan::gettimeofday();

    unsigned sum = 0;
    if (cached)
    {
      for (unsigned n = 1; n < max; ++n)
      {
        tntdb::Statement stmt = conn.prepareCached(
          "select CATEGORY from PRODUCTS where PROD_ID = :prodId");
        stmt.set("prodId", n);
        sum += stmt.selectValue().getUnsigned();
      }
    }
    else if (prepared)
    {
      tntdb::Statement stmt = conn.prepare(
        "select CATEGORY from PRODUCTS where PROD_ID = :prodId");
      for (unsigned n = 1; n < max; ++n)
      {
        stmt.set("prodId", n);
        sum += stmt.selectValue().getUnsigned();
      }
    }
    else
    {
      for (unsigned n = 1; n < max; ++n)
      {
        tntdb::Statement stmt = conn.prepare(
          "select CATEGORY from PRODUCTS where PROD_ID = :prodId");
        stmt.set("prodId", n);
        sum += stmt.selectValue().getUnsigned();
      }
    }

    cxxtools::Timespan t1 = cxxtools::Timespan::gettimeofday();

    std::cout << "count=" << max << "\n"
                 "sum=" << sum << "\n"
                 "t=" << (t1 - t0) << std::endl;

  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
