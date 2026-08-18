# Selecting data

A result from a select statement is represented by three classes: `tntdb::Result`,
`tntdb::Row`, and `tntdb::Value`. The result is a collection of rows, and each row is
a collection of values. `tntdb::Result` and `tntdb::Row` both act like a vector with random
access. Both have a operator `[]` to access the underlying object, alternatively the objects
can be iterated using the C++ for loop.

Most methods also provide shorthand operators for syntactic sugar and
shorter expressions.

## Value

A `tntdb::Value` holds a single value that may also be `null`. The best way to fetch the
actual value from a `tntdb::Value` is to use the method `get`, which expects a
reference to a variable as a parameter where the value is stored. It returns a
bool, which is true if the actual value was not null.

```cpp
    tntdb::Value value = ...;
    int integerValue;
    value.get(integerValue);

    tntdb::Value anotherValue = ...;
    std::string stringValue;
    anotherValue.get(stringValue);
```

A row also has a get method, which expects a reference and optionally another
reference to a bool, which is filled with true, if the value is not null or
false if the value is null. The method returns a special helper object
`tntdb::RowReader` which has the same get methods and maintains a column
counter. This makes it easy to loop through all values in a row.

```cpp
    tntdb::Row::row = ...;

    int integerValue;
    std::string stringValue;

    row.get(integerValue)
       .get(stringValue);
```

Shorthand operators are available as syntactic sugar:

```cpp
    row >> integerValue
        >> stringValue;
```


## selecting Result

A resultset is a collection of rows with columns. This is represented in Tntdb
with a class tntdb::Result. To get a result the method `select` of
tntdb::Connection can be used.

```cpp
    auto result = conn.select("select col1, col2 from foo");
    for (auto row: result)
    {
        ... do something with row
    }

```

If a statement returns just one single row, the method selectRow can be used.
It fetches just the first row or, if none is found throws `tntdb::NotFound`:

```cpp
    auto row = conn.selectRow("select col1, col2 from foo where id = 5");
    row.get(col1)
       .get(col2);
```

If just a single row with a single column is expected there is a method
selectValue:

```cpp
    auto row = conn.selectValue("select count(*) from foo");
    unsigned count;
    row.get(count);
```

or shorter using operator:

```cpp
    row >> count;
```

## Using prepared statements

Mostly the better option for using sql is to use prepared statements. They are
parsed just once and may contain variables, which can be filled before
execution. A prepares statement is represented by the class `tntdb::Statement`
and is created with the method `prepare` of `tntdb::Connection`.

The use of variables is strongly encouraged to prevent sql injection. Tntdb
takes care of passing the values as needed either by escaping the values
properly or using native features of the underlying database.

Tntdb uses always named variables. Those are specified by a name prefixed by a
colon. The actual value of the statement can then be set using the method `set`
method of `tntdb::Statement`. The class has also the methods `selectRow` and
`selectValue` similar to the once in `tntdb::Connection`.

```cpp
    auto stmt = conn.prepare("select col1, col2 from foo where id = :id");
    stmt.set("id", 5);
    auto row = stmt.selectRow();
```

To fetch all rows using a cursor the statement can be iterated with a C++ for
loop. When the iteration is started, the statement is executed and the first
row is fetched. With each iteration the next row is returned.

```cpp
    auto stmt = conn.prepare("select col1, col2 from foo");
    for (auto row: result)
    {
        ... do something with row
    }
```

The great advantage compared to tntdb::Result is, that the result is not held
in memory. Just the current row is visible (although the driver will fetch
multiple rows at once internally for better performance).

Some shorthand operators makes it even shorter:

And also for setting the variables:

```cpp
    auto stmt = conn("select col1, col2 from foo where id = :id");
    stmt("id", 5);
    auto row = stmt.selectRow();
```

Which can be chained to:

```cpp
    auto row = conn("select col1, col2 from foo where id = :id")
                   ("id", 5)
                   .selectRow();
```

Or when iterating:

```cpp
    for (auot row: conn("select col1, col2 from foo where col3 > :someValue")
                       ("someValue", 6))
    {
        ... do something with row
    }
```
