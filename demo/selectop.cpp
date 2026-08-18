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

        auto db = tntdb::connect(argv[1]);

        // selecting a single value
        //
        // tntdb::Connection::operator() (const std::string&) prepares a statement
        // and returns it (tntdb::Statement).
        //
        // tntdb::Statement::selectValue executes the statement and selects the
        // first value of the first row as a tntdb::Value
        //
        // tntdb::Value::operator() (T&) extracts the value and fills the
        // passed variable It returns a bool whether the value was not null
        // (which is ignored here)
        //
        unsigned count;
        db("select count(*)"
           "  from foo").selectValue()      // returns a tntdb::Value
          (count);                          // extracts unsigned value from tntdb::Value

        // selecting multiple values of a row
        //
        // tntdb::Statement::selectRow() selects the first row as tntdb::Row.
        // tntdb::Row::operator(T&) extracts the value and returns a tntdb::RowReader,
        // which tracks the column number.
        // tntdb::RowReader::operator() (T&) extracts the current column, increases the
        // column counter by 1 and returns the row reader.
        unsigned max, min;
        db("select max(a), min(a) from foo").selectRow()
          >> max
          >> min;

        // tndb::Statement::select(const std::string&) executes the statement
        // and returns a tntdb::Result, which can be iterated. Each iteration
        // returns a tntdb::Row.
        auto result = db("select a, b from foo").select();
        for (auto row: result)
        {
            unsigned a;
            std::string b;

            row >> a
                >> b;

            // or using std::optional which handles nullable values nicely
            {
                std::optional<unsigned> a;
                std::optional<std::string> b;
                row >> a
                    >> b;
            }
        }

        // Better than tntdb::Result is a cursor.
        //
        // tntdb::Connection::operator() (const std::string&) prepares a statement
        // and returns it (tntdb::Statement). Here we just iterates over it.
        // starting iteration executes the prepared statement and fetches the
        // first row using a db cursor.
        //
        // Note that in this case the result is not held in memory as in tntdb::Result.
        //
        auto sel = db("select a, b from foo");
        for (auto row: result)
        {
            // do something with the row
        }

        // A raw string helps formatting the statement properly
        // tntdb::Statement::operator() (const std::string&, T) sets the value of a variable.
        // Iteration creates a cursor over the results.
        for (auto row: db(R"SQL("
                  select a, b
                    from foo"
                  where a > :a
                )SQL")("a", 42))
        {
            unsigned a;
            std::string b;

            row >> a >> b;  // fetch values
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

