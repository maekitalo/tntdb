#ifndef TNTDB_TESTBASE_H
#define TNTDB_TESTBASE_H

#include <cxxtools/unit/testsuite.h>
#include <tntdb/connection.h>
#include <tntdb/statement.h>
#include <stdlib.h>

class TntdbTestBase : public cxxtools::unit::TestSuite
{
protected:
    tntdb::Connection conn;
    tntdb::Statement del;

public:
    explicit TntdbTestBase(const std::string& name)
        : cxxtools::unit::TestSuite(name)
        { }

    void setUp();
    void tearDown();
};

#endif
