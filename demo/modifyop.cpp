/*

 This demonstrates how to modify data in a database with tntdb using operator ().

 */

#include <iostream>
#include <cxxtools/log.h>
#include <tntdb/connect.h>
#include <tntdb/pscconnection.h>
#include <tntdb/statement.h>
#include <tntdb/transaction.h>

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

        // insert
        //
        // tntdb::Connection::operator() (const std::string&) prepares a statement
        // and returns it (tntdb::Statement).
        // tntdb::Statement::operator() (const std::string&, T) sets the value of a variable
        // and returns *this for chaining
        // tntdb::Statement::operator() () executes the statement.
        db("insert into foo(a, b)"
           " values (:a, :b)")
          ("a", 42)
          ("b", "Hi there")
          ();

        // update
        //
        // Raw multiline strings helps formatting sql statements nicely
        db(R"SQL(
            update foo
               set b = :b
             where a = :a
           )SQL")
          ("a", 42)
          ("b", "Hello")
          ();

        // delete
        db("delete from foo"
           " where a = :a")
           ("a", 42)
           ();

        // using a transaction
        {
            // We define the transaction object in a scope.
            //
            // The transaction will be rolled back automatically in case the
            // end of the block is reached and no commit or rollback is called.
            // This happens typically when a exception is thrown.
            //
            // Note that when the rollback fails in the destructor errors are lost.

            tntdb::Transaction transaction(db);
            auto ins = db("insert into foo(a, b)"
                          " values (:a, :b)");

            // a prepared statement can be executed multiple times
            ins("a", 42)
               ("b", "Hi there")
               ();

            ins("a", 43)
               ("b", "Hello")
               ();

            // the previous value of b remains
            ins("a", 44)
               ();

            transaction.commit();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

