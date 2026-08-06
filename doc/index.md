# Tntdb - a C++ Database access library

Tntdb offers easy access to databases using SQL with C++. It supports access to
PostgreSQL, MySql, Sqlite3 and Oracle using a driver architecture.

## Basic Classes

For each item in a relation database tntdb offers classes. The main classes are:

    - tntdb::Connection                     a database connection
    - tntdb::Result                         a database result (rows and columns)
    - tntdb::Row                            a single row
    - tntdb::Value                          a single db value
    - tntdb::Statement                      a prepared statement
    - tntdb::Statement::const\_iterator     a database cursor
    - tntdb::Transaction                    a database transaction

All classes are wrappers, which held a shared pointer to the actual
implementation. So when copying the copy references the very same object. When
the last instance of a object is deleted (typically by leaving the scope) the
actual implementation is released.

To use those, each has its own header, which must be included when needed:

    - `#include <tntdb/connection.h>`
    - `#include <tntdb/result.h>`
    - `#include <tntdb/row.h>`
    - `#include <tntdb/value.h>`
    - `#include <tntdb/statement.h>`
    - `#include <tntdb/transaction.h>`

As an alternative there is a header `<tntdb.h>`, which includes allof them.

## Connecting to a database

To get a connection a single function `tntdb::connect` is offered. It expects a
url as a parameter. The url starts always with the driver name and a colon. The
rest is driver specific. Here is an example to get a connection to a postgresql
database:

```cpp
    auto conn = tntdb::connect("postgresql:dbname=foo");
```

The function can be found in the header `<tntdb/connect.h>`

A very special replicate driver is also available for simple replication
between databases.

See [Connecting to a db](connect.md) for details.

## Selecting data

There are several ways to select data. You can select result sets, rows or just values.

To select a result set we use tntdb::Connection::select(const std::string&) and iterate
over it:

```cpp
    auto result = conn.select("select firstname, lastname from person");
    std::string firstname, lastname;
    for (auto row: result)
    {
        row.get(firstname)
           .get(lastname);
    }
```

See [Selecting data](select.md) for details.

## Executing statments

Executing update, insert or delete is easy. A method tntdb::Connection::execute
is offered:

```cpp
    conn.execute("update mytable set column='value' where id=50");
```

A prepared statement can be used to pass the values using variables:

```cpp
    auto upd = conn.prepare("update mytable set column=:column where id=:id");
    upd.set("id", 50)
       .set("column", "value")
       .execute();
```

See [Executing modification statements](modify.md) for details.

## Using date, time and datetime

Since date, time and datetime are somewhat difficult to handle database indipenently
Tntdb offers simple types for that:

    - tntdb::Date
    - tntdb::Time
    - tntdb::Datetime

Note that those are just interface classes and not check for valid values.
To insert e.g. a date into a database we use tntdb::Date:

```cpp
    auto ins = conn.prepare("insert into mytable (id, date) values (:id, :date)");
    ins.set("id", 60)
       .set("date", tntdb::Date(2026, 8, 6))
       .execute();
```

See [Date and time support](datetime.md) for details.

## Decimal type

Similar to date, time and datetime there is a decimal type tntdb::Decimal, which represents
decimal numbers.

## Blobs

Tntdb supports also blobs (binary large objects) with a helper class `tntdb::Blob`.

See [Blobs](blobs.md) for details.

## Custom data types

Tntdb allows to implement custom conversions for own datatypes, so that they
can be easily used in tntdb. Note that only single values are supported.

See [Custom data types](customtypes.md) for details.

## Transactions

Transactions are supported by the class `tntdb::Transaction`. The object is
typically instantiated with a connection parameter on the stack in which case
the transaction is started. Using `tntdb::Transaction::commit` the transaction
is committed. If the transaction is not explictly ended before the destructor,
a rollback is issued there.

See [Transactions](transaction.md) for details.

## Error handling

In case of errors a exception of type `tntdb::Error` is thrown, which is
derived from `std::runtime_error`. Normally the exception is caused by the
driver itself. A special case is the error `tntdb::NotFound`, which is throw by
directly from Tntdb. It is throw, if `selectRow` or `selectValue` fail to fetch
a row.

## Logging

For logging cxxtools is used.

See [Loggin with cxxtools](loggin.md) for details.
