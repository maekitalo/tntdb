# Selecting data

A query result is represented by three classes: `tntdb::Result`, `tntdb::Row`,
and `tntdb::Value`. A result contains rows, and each row contains values. Both
`tntdb::Result` and `tntdb::Row` provide indexed access and can be iterated
using a C++ range-based `for` loop.

Tntdb also provides shorthand operators for frequently used operations.

`tntdb::Connection::select()` expects a row-producing query. It does not parse
or enforce SQL statement type before passing the query to the database driver.
If a non-select statement is passed (e.g. `delete`), behavior is
driver-dependent: the statement may execute and then either return an empty
result or throw an exception. Therefore use `execute()` for modifying
statements and reserve `select()` for read-only queries.

## Value

A `tntdb::Value` represents a single database value, which may be SQL `NULL`.
The `get()` method stores the value in the supplied variable and returns `true`
when the database value is not `NULL`.

```cpp
tntdb::Value value = ...;
int integerValue;

if (value.get(integerValue))
{
    // integerValue contains the database value
}

std::string stringValue;
anotherValue.get(stringValue);
```

If the database value is NULL, get() returns false and leaves the supplied
variable unchanged.

## Reading a row

`tntdb::Row::get()` returns a `tntdb::RowReader`, which keeps track of the
current column. This makes it convenient to read several columns in sequence.

```cpp
tntdb::Row row = ...;
int integerValue;
std::string stringValue;

row.get(integerValue)
   .get(stringValue);
```

The extraction operator provides a shorter equivalent:

```cpp
row >> integerValue
    >> stringValue;
```

## Selecting a result

Use `tntdb::Connection::select()` to materialize a complete result set:

```cpp
auto result = conn.select("select col1, col2 from foo");

for (auto row : result)
{
    // process row
}
```

If exactly one row is expected, use `selectRow()`. It returns the first row and
throws `tntdb::NotFound` if no row is found.

```cpp
auto row = conn.selectRow("select col1, col2 from foo where id = 5");
row.get(col1)
   .get(col2);
```

If a single value is expected, use `selectValue()`:

```cpp
auto value = conn.selectValue("select count(*) from foo");
unsigned count;
value.get(count);
```

or, using the shorthand operator:

```cpp
value >> count;
```

## Prepared statements

Most of the time, queries need to be parameterized.

SQL statements could be constructed using string concatenation, for example
with std::ostringstream, but this is not recommended. The application would
then have to handle special characters correctly to avoid misinterpretation
of data and, in particular, SQL injection.

Prepared statements solve this problem by separating the SQL statement from
its parameter values. They can also improve performance when the same statement
is executed multiple times with different parameter values. Parsing can be
performed either on the client side or on the server side. Tntdb lets the
driver decide how the query is prepared and which placeholders are required
by the database.

To create a prepared statement, tntdb::Connection provides prepare(), which
takes the query as a std::string and returns a tntdb::Statement. The query can
contain named parameters prefixed with a colon. A parameter may occur multiple
times in a query. The Statement class provides setter methods for passing
parameter values of different types.


```cpp
auto stmt = conn.prepare("select col1, col2 from foo where id = :id");
stmt.set("id", 5);
auto row = stmt.selectRow();
```

`selectRow()` and `selectValue()` are also available on tntdb::Statement as
well as execute() when [modify.md](modifing data).


## Iterating over a statement

A statement can be iterated directly. Iteration executes the statement and
fetches rows as needed instead of keeping the complete result in memory.

```cpp
auto stmt = conn.prepare("select col1, col2 from foo");

for (auto row : stmt)
{
    // process row
}
```

The driver may fetch multiple rows internally for efficiency, but only the
current row is exposed through the iterator.

The number of rows fetched at once can be passed as a value to
`tntdb::Statement::begin(unsigned)` if you use an explicit iterator. The
default is 100.

```cpp
auto stmt = conn.prepare("select col1, col2 from foo");

for (auto it = stmt.begin(10); it != stmt.end(); ++it)
{
    auto row = *it;
    // process row
}
```


### Result or statement?

Use `tntdb::Result` when you want the complete result set materialized and
available for random access. Iterate a `tntdb::Statement` when you want
cursor-style processing without retaining the complete result set in memory.

## Shorthand syntax

`tntdb::Connection` and `tntdb::Statement` provide shorthand operators:

```cpp
auto stmt = conn("select col1, col2 from foo where id = :id");
stmt("id", 5);
auto row = stmt.selectRow();
```

The calls can be chained:

```cpp
auto row = conn("select col1, col2 from foo where id = :id")
               ("id", 5)
               .selectRow();
```

Statements can also be iterated directly:

```cpp
for (auto row : conn("select col1, col2 from foo where col3 > :someValue")
                     ("someValue", 6))
{
    // process row
}
```
