# Transactions

Transactions can be handled manually or using the transaction
`tntdb::Transaction`class of tntdb which is the preferred way.

A transaction can be started by calling `tntdb::Connection::beginTransaction()`
and finished either by committing with `tntdb::Connection::commitTransaction()`
or rolled back using `tntdb::Connection::rollbackTransaction()`.

But that is mostly not exception save. In case of an exception the
transaction may be kept open. A better approach is to use the transaction
class and put it on the stack.

The object keep track of the transaction state and if the destructor
detects, that the transaction is not ended, it will roll it back.

### Example

```cpp
    void foo(tntdb::Connection conn)
    {
        tntdb::Transaction transaction(conn);
        conn.execute("update foo ...");
        conn.execute("update bar ...");
        transaction.commit();
    }
```

Here both updates are executed inside the transaction. If one of them
fails (throws an exception), the transaction is implicitly rolled back.

## Nested transactions

The above is not exactly precise when it comes to tntdb. Actually a tntdb
connection maintains a transaction counter. Every time `beginTransaction()` is
called, the counter is increased. Calling `commitTransaction()` or
`rollbackTransaction()` decreases the counter and does the action only, when
the counter reaches 0. So the last call does the actual commit or rollback.

This is done in the assumption, that transaction objects can be
nested and if somewhere a exception occurs, the whole call chain
is rolled back and the last transaction wins.

### Example

```cpp
    void foo(tntdb::Connection conn)
    {
        tntdb::Transaction transaction(conn);
        conn.execute("update ...");     // update foo
        transaction.commit();
    }

    void bar(tntdb::Connection conn)
    {
        tntdb::Transaction transaction(conn);
        foo(conn);
        conn.execute("update ...");     // update bar
        transaction.commit();
    }
```

Here when calling `foo` and the update succeeds, the transaction
is normally committed.

But when calling `bar` there may be a problem. If foo succeeds and the update
in bar fails, the whole transaction should be rolled back. Since the commit in
foo did nothing here but just decreased the transaction counter, the
transaction in bar can roll back the whole transaction.

Without the transaction counter, foo would have already committed the
transaction before bar has done its work.
