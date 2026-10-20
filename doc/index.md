# Tntdb - a C++ database access library

Tntdb provides convenient access to SQL databases from C++. It supports
PostgreSQL, MySQL, SQLite, and Oracle through a driver architecture.

## Quick start

```cpp
#include <tntdb/connect.h>

int main()
{
    auto conn = tntdb::connect("postgresql:dbname=foo");

    auto stmt = conn.prepare("select name from person where id = :id");
    stmt.set("id", 42);

    std::string name;
    stmt.selectValue().get(name);
}
```

## Basic classes

Tntdb provides C++ classes for the fundamental concepts of a relational
database. Most Tntdb classes are lightweight wrappers around a shared
implementation object. Copying such an object therefore references the same
underlying implementation. The implementation is released when the last wrapper
is destroyed.

Each class has its own header. Alternatively, `<tntdb.h>` includes the complete
public interface.

## Connecting to a database

Use `tntdb::connect()` to create a connection. It expects a database URL whose
prefix identifies the driver; the remainder is driver-specific.

```cpp
auto conn = tntdb::connect("postgresql:dbname=foo");
```

The function is declared in `<tntdb/connect.h>`.

A special `replicate` driver is also available for simple replication between
databases.

See [Connecting to a database](connect.md) for details.

## Selecting data

Tntdb can select complete result sets, individual rows, or single values.

```cpp
auto result = conn.select("select firstname, lastname from person");

std::string firstname;
std::string lastname;

for (auto row : result)
{
    row.get(firstname)
       .get(lastname);
}
```

See [Selecting data](select.md) for details.

## Executing statements

`INSERT`, `UPDATE`, and `DELETE` statements can be executed using
`tntdb::Connection::execute()`:

```cpp
conn.execute("update mytable set column='value' where id=50");
```

For variable values, prepared statements with bound parameters are recommended:

```cpp
auto upd = conn.prepare("update mytable set column=:column where id=:id");
upd.set("id", 50)
   .set("column", "value")
   .execute();
```

See [Executing modification statements](modify.md) for details.

## Date, time, and datetime

Date and time syntax differs between database systems. Tntdb provides
database-independent types for these values.

```cpp
auto ins = conn.prepare("insert into mytable (id, date) values (:id, :date)");
ins.set("id", 60)
   .set("date", tntdb::Date(2026, 8, 6))
   .execute();
```

See [Date and time support](datetime.md) for details.

## Decimal type

`tntdb::Decimal` represents decimal numbers in a database-independent way.

## Blobs

Tntdb supports binary large objects through `tntdb::Blob`.

See [Blobs](blobs.md) for details.

## Custom data types

Tntdb allows applications to define conversions for custom data types so they
can be used naturally with statements and result values. Custom conversions
operate on individual database values.

See [Custom data types](customtypes.md) for details.

## Transactions

Transactions are supported by `tntdb::Transaction`. Constructing a transaction
object starts the transaction. Calling `commit()` commits it. If the object
leaves scope before the transaction is explicitly completed, it is rolled back.

See [Transactions](transaction.md) for details.

## Error handling

Database errors are reported using `tntdb::Error`, which derives from
`std::runtime_error`. `tntdb::NotFound` is a special exception thrown when
`selectRow()` or `selectValue()` cannot fetch a row.

## Logging

Tntdb uses cxxtools for logging.

See [Logging with cxxtools](logging.md) for details.

## Prepared statement cache connection

Preparing a statement has some overhead. A prepared statement can be stored and
reused, or applications can use `tntdb::PSCConnection`, which provides an
automatic prepared-statement cache.

```cpp
tntdb::PSCConnection conn = tntdb::connect("...");
auto stmt = conn.prepare("..."); // uses the cache
```

## Serial columns and generated IDs

Database systems use different mechanisms for automatically generated primary
keys. Tntdb provides `tntdb::Connection::lastInsertId()` to retrieve the
identifier assigned by the last insert operation.

Some databases, such as PostgreSQL and Oracle, use named sequences. MySQL and
SQLite do not require such a sequence name and ignore it when supplied.

```cpp
void insertData(tntdb::Connection conn)
{
    auto ins = conn.prepare(
        "insert into foo (name) "
        "values (:name)");

    ins.set("name", "some name")
       .execute();

    std::cout << "id: " << conn.lastInsertId("foo_id_seq") << std::endl;
    std::cout << "id: " << conn.lastInsertId() << std::endl;
}
```
