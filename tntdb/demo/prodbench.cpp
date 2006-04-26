#include <tntdb/connect.h>
#include <tntdb/result.h>
#include <tntdb/value.h>
#include <cxxtools/arg.h>
#include <cxxtools/hirestime.h>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> prepared(argc, argv, 'p');
    cxxtools::Arg<bool> cached(argc, argv, 'c');

    tntdb::Connection conn = tntdb::connect("mysql:db=DS2;user=web;passwd=web");
    unsigned max = conn.selectValue("select max(PROD_ID) from PRODUCTS")
                       .getUnsigned();

    cxxtools::HiresTime t0 = cxxtools::HiresTime::gettimeofday();

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

    cxxtools::HiresTime t1 = cxxtools::HiresTime::gettimeofday();

    std::cout << "count=" << max << "\n"
                 "sum=" << sum << "\n"
                 "t=" << (t1 - t0) << std::endl;

  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
