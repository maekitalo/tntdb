### The replication driver

The replication driver is a special case, since it does not handle a specific database
but implements a simple replication at the application level. To use it, the
connection string must start with "replicate:" followed by a list of connection
strings separated by '|'.

A connection is established to each database.

Statements that just read data use the first connection in the list.

Statements that modify data execute the statement on each connection. Before
the actual statement is executed, a transaction is started on each connection
unless there is already an active transaction. If the statement fails on one
connection, all transactions are rolled back to ensure consistency.

For modification statements the connection urls are ordered alphabetically to
prevent deadlocks when different applications use the replication driver with
different ordering of database urls.

Note that the connections of the replication driver do not need to use the same
database drivers but can use a mix of different drivers, for example you can
replicate between a local SQLite and a PostgreSQL database.
When replicating between different databases you are
limited to using sql statements which all involved databases understand.

Example connect for the replication driver between a SQLite and PostgreSQL Database:

    tntdb::Connection conn =
      tntdb::connect("replicate:sqlite:mydatabase.db|postgresql:dbname=DS2 user=web password=web");

This connection reads data from a local SQLite database file and writes to the
sqlite and PostgreSQL database.

#### Replication semantics and limits

The replication driver is a synchronous, application-level fan-out mechanism.
It is useful to keep multiple databases in sync for simple setups, but it is
not a database-native replication system.

