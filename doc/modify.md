# Executing modification statements

The simplest way to execute a modification statement is
`tntdb::Connection::execute()`:

```cpp
conn.execute("update foo set col1 = col1 + 1 where col3 > 5");
```

This is suitable when the SQL statement is static and does not require variable
input. For variable or user-supplied values, use prepared statements with bound
parameters instead.

## Using prepared statements

As described in [Selecting data](select.md), Tntdb supports prepared statements
with named parameters. `tntdb::Statement::execute()` executes the statement and
returns the number of affected rows as an `unsigned` value.

```cpp
auto upd = conn.prepare("update foo set col1 = :newValue where id = :id");

upd.set("id", 5)
   .set("newValue", "something")
   .execute();

upd.set("id", 6)
   .set("newValue", "something else")
   .execute();
```

Using the shorthand operators, the same code can be written as:

```cpp
auto upd = conn("update foo set col1 = :newValue where id = :id");

upd("id", 5)
   ("newValue", "something")
   ();

upd("id", 6)
   ("newValue", "something else")
   ();
```
