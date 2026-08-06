# Executing modification statements

The simplest form to execute a modification statement is to use the
method `execute` of `tntdb::Connection`:

```cpp
    conn.execute("update col1 = col1 + 1 where col3 > 5");
```

This is suitable if no variables are needed. If you need some input, it is
strongly encouraged to use prepared statements and variables instead.

## Using prepared statements

As explained in [Selecting data](select.md) Tntdb supports preparation of
statements with variables. The `tntdb::Statement` has a method `execute` to
execute a statement. It returns the number of affected rows as a `unsigned`.

```cpp
    auto upd = conn.prepare("update col1 = :newValue where id = :id");
    upd.set("id", 5)
       .set("newValue", "something")
       .execute();

    upd.set("id", 6)
       .set("newValue", "something else")
       .execute();
```

Using shorthand operator it looks like this:

```cpp
    auto upd = conn("update col1 = :newValue where id = :id");
    upd("id", 5)
       ("newValue", "something")
       ();

    upd("id", 6)
       ("newValue", "something else")
       ();
```
