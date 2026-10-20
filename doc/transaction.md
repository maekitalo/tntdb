# Transactions

Transactions can be controlled manually through `tntdb::Connection`, but the
preferred approach is `tntdb::Transaction`.

A transaction can be started with `tntdb::Connection::beginTransaction()` and
completed with `commitTransaction()` or `rollbackTransaction()`. Manual
transaction management, however, is not exception-safe: if an exception is
thrown before the transaction is completed, it may remain open.

`tntdb::Transaction` uses RAII to make transaction handling exception-safe.
Constructing the object starts a transaction. If the object is destroyed while
the transaction is still active, it rolls the transaction back automatically.

## Example

```cpp
void foo(tntdb::Connection conn)
{
    tntdb::Transaction transaction(conn);

    conn.execute("update foo ...");
    conn.execute("update bar ...");

    transaction.commit();
}
```

Both updates run in the same transaction. If either update throws an exception,
stack unwinding destroys `transaction` and the transaction is rolled back
automatically.

## Nested transactions

Tntdb maintains a transaction counter for each connection. Every call to
`beginTransaction()` increments the counter. Calls to `commitTransaction()` or
`rollbackTransaction()` decrement it, and the actual commit or rollback is
performed when the counter reaches zero.

This allows transaction objects to be nested. A function can use a transaction
without knowing whether its caller has already started one.

```cpp
void foo(tntdb::Connection conn)
{
    tntdb::Transaction transaction(conn);
    conn.execute("update ..."); // update foo
    transaction.commit();
}

void bar(tntdb::Connection conn)
{
    tntdb::Transaction transaction(conn);

    foo(conn);
    conn.execute("update ..."); // update bar

    transaction.commit();
}
```

When `foo()` is called by itself, its `commit()` completes the transaction
normally. When `foo()` is called from `bar()`, the inner commit only decreases
the transaction counter. If the update in `bar()` then fails, the outer
transaction can still roll back the complete transaction.

Without the transaction counter, `foo()` would commit before `bar()` had
finished its work.
