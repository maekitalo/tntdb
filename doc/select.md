# Selecting data

A query result is represented by three classes: `tntdb::Result`, `tntdb::Row`,
and `tntdb::Value`. A result contains rows, and each row contains values. Both
`tntdb::Result` and `tntdb::Row` provide indexed access and can be iterated
using a C++ range-based `for` loop.

Tntdb also provides shorthand operators for frequently used operations.

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

Prepared statements are recommended whenever values are supplied separately
from the SQL text. They avoid SQL string concatenation and help prevent SQL
injection.

A prepared statement is represented by `tntdb::Statement` and created with
`tntdb::Connection::prepare()`. Tntdb uses named parameters, prefixed by a
colon.

```cpp
auto stmt = conn.prepare("select col1, col2 from foo where id = :id");
stmt.set("id", 5);
auto row = stmt.selectRow();
```

`selectRow()` and `selectValue()` are also available on `tntdb::Statement`.

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
