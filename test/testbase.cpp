#include "testbase.h"
#include <tntdb/connect.h>
#include <cxxtools/log.h>

log_define("tntdb.unit")

void TntdbTestBase::setUp()
{
    if (!conn)
    {
        const char* dburl = getenv("TNTDBURL");
        if (!dburl)
          dburl = "sqlite:test.db";

        const char* user = getenv("TNTDBUSER");
        const char* password = getenv("TNTDBPASSWORD");
        if (!user)
            user = "";
        if (!password)
            password = "";

        log_info("testing with dburl=" << dburl);

        conn = tntdb::connect(dburl, user, password);

        del = conn.prepare("delete from tntdbtest");
        del.execute();
    }
}

void TntdbTestBase::tearDown()
{
    del.execute();
}
